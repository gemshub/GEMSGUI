#include <math.h>
#include "s_lsm.h"

// new servise part
static fd_type maxarg1, maxarg2;
#define FMAX(a,b)(maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ? (maxarg1) : (maxarg2))
//static fd_type minarg1, minarg2;
//#define FMIN(a,b)(minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ? (minarg1) : (minarg2))

static fd_type sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
(iminarg1) : (iminarg2))

TSVDcalc::TSVDcalc( double *aPar, TLMDataType *aData ):
   data(aData), par(aPar), A(0), b(0)
{
    m = aData->getM_dat();
    n = aData->getN_par();
    U = 0;
    w = 0;
    V = 0;
    CVM = 0;
    chisq = 0.0;
}

TSVDcalc::TSVDcalc( short M, short N, float *aA, double *ab, double *aX ):
   m(M), n(N), data(0), par(aX), A(aA), b(ab)
{
    U = 0;
    w = 0;
    V = 0;
    CVM = 0;
    chisq = 0.0;
}


TSVDcalc::~TSVDcalc()
{
  free_arrays();
}

// *** main function.
void TSVDcalc::CalcMin( double *sdpar )
{
// int i, j;

#ifdef IPMGEMPLUGIN
    if( data->getInfo() == -1 ) //test_sizes
      return;
#endif

  alloc_arrays();
  svdMin( par, U, V, w, chisq );

  data->lm_print_default( par, 0, -1, 0,0,chisq );

  svdStat( V, w, CVM);

// f_out << "cvm = " << endl;
// for( i=0; i<n; i++ )
// {  for( j=0; j<n; j++ )
//    f_out << cvm(i,j) << " ";
//   f_out << endl;
// }

 free_arrays();
}


// *** main function.
int TSVDcalc::CalcSVD(  bool transp )
{
 int i, j;

 fstream f_out("svd.out", ios::out  );
 if( !f_out.good() )
   return -1;

  alloc_arrays();

 if( transp )     // transpose input matrix  A
 {
   for(i=0;i<m;i++) // Copy a into u if you don’t want it to be destroyed
   for(j=0;j<n;j++)
    u(i,j)= (A[(j)*m+(i)]);
 }
 else
 { for(i=0;i<m;i++) // Copy a into u if you don’t want it to be destroyed
   for(j=0;j<n;j++)
    u(i,j)=a(i,j);
 }

 if( svdGetUWV(U,w,V) < 0 )
     return -1; // SVD of the square matrix a.

 // output u, v, w to file
 f_out << "u = " << endl;
 for( i=0; i<m; i++ )
 {  for( j=0; j<n; j++ )
    f_out << u(i,j) << " ";
   f_out << endl;
 }
 f_out << "w = " << endl;
 for( j=0; j<n; j++ )
    f_out << w[j] << " ";
 f_out << endl;
 f_out << "v = " << endl;
 for( i=0; i<n; i++ )
 {  for( j=0; j<n; j++ )
    f_out << v(i,j) << " ";
   f_out << endl;
 }

  int wj_zero = 0;
  double wmin, wmax=0.0; // Will be the maximum singular value obtained.
  for(j=0;j<n;j++)
     if (w[j] > wmax) wmax=w[j];
// This is where we set the threshold for singular values allowed to be nonzero.
// The constant  is typical, but not universal.
// You have to experiment with your own application.
  wmin=wmax*1.0e-6;
  for(j=0;j<n;j++)
    if (w[j] < wmin)
      {  wj_zero = (j+1);
         w[j]=0.0;
      }
  svdGetX(U,w,V,b,par); // Now we can backsubstitute.

  if( wj_zero && transp )
    svdGetXmore0( wj_zero-1, V, par ); // Now we get x with elements >=0
// output vectors w, x
 f_out << "w = " << endl;
 for( j=0; j<n; j++ )
    f_out << w[j] << " ";
 f_out << endl;
 f_out << "x = " << endl;
 for( j=0; j<n; j++ )
    f_out << par[j] << " ";
 f_out << endl;

 free_arrays();
 return wj_zero;
}


void TSVDcalc::CalcB( bool transp, fd_type *bb )
{
  int j,i;
  float aa;

  for( i=0; i<m; i++ )
  {
    bb[i] = 0;
    for( j=0; j<n; j++ )
    {
       if( transp )     // transpose input matrix  A
         aa = (A[(j)*m+(i)]);
      else
         aa = a(i,j);
       bb[i] += aa * par[j];
     }
  }
}



// internal functions
//--------------------------------------------------------------------------
// *** allocate work space.
void TSVDcalc::alloc_arrays( )
{
    U = new fd_type[m*n];
    w = new fd_type[n];
    V = new fd_type[n*n];
    CVM = new fd_type[n*n];

    if (!U || !w || !V || !CVM )
            control.info = 9;

}

// *** free work space.
void TSVDcalc::free_arrays( )
{
  if( U )
    { delete[] U; U = 0; }
  if( w )
    { delete[] w; w = 0; }
  if( V )
    { delete[] V; V = 0; }
  if( CVM)
    { delete[] CVM; CVM = 0; }
 }


void TSVDcalc::svdGetXmore0( int ii, fd_type *V, fd_type x[])
{
 int j, jmin=0, jmax=0;
 fd_type a1, max=0, min=0;

 for (j=0;j<n;j++)
   if( x[j] < 0 )
      break;
 if( j == n ) // all elements x more 0
   return;

 for (j=0;j<n;j++)   //Select intervals
 {
   if( v(j,ii) > 0 )
   {
     a1 = (-x[j])/v(j,ii);
     if( !jmin || a1 > min )
     {
       min = a1; jmin = j+1;
     }
   }
   if( v(j, ii) < 0 )
   {
     a1 = (-x[j])/v( j, ii);
     if( !jmax || a1 < max )
     {
       max = a1; jmax = j+1;
     }
   }
 }
 if( min > max ) // no solutions in system
   return;

 a1 = (min+max)/2;
 for (j=0;j<n;j++)  // X multiply by a*V to get answer.
    x[j]  += v(j, ii)*a1;
}


void TSVDcalc::svdGetX(fd_type *U, fd_type w[], fd_type *V,
                          fd_type b[], fd_type x[])
// Solves AÂ·X = B for a vector X, where A is specified by the arrays
// u[1..m][1..n], w[1..n],  v[1..n][1..n] as returned by svdcmp.
// m and n are the dimensions of a, and will be equal for
// square matrices. b[1..m] is the input right-hand side.
//x[1..n] is the output solution vector.
// No input quantities are destroyed
{
  int jj,j,i;
  fd_type s,*tmp;
  tmp = new fd_type[n]; // vector(1,n);

 for (j=0;j<n;j++)   // Calculate UTB.
 {
    s=0.0;
    if (w[j])  // Nonzero result only if wj is nonzero.
    {  for (i=0;i<m;i++)
         s += u(i,j)*b[i];
       s /= w[j]; // This is the divide by wj .
    }
   tmp[j]=s;
 }
 for (j=0;j<n;j++)  // Matrix multiply by V to get answer.
 {
     s=0.0;
     for (jj=0;jj<n;jj++)
        s += v(j,jj)*tmp[jj];
     x[j]=s;
  }
delete[] tmp; // free_vector(tmp,1,n);
}


fd_type TSVDcalc::pyt_hag(fd_type a, fd_type b)
// Computes (a2 + b2)1/2 without destructive underflow or overflow.
{
  fd_type absa,absb;
  absa=fabs(a);
  absb=fabs(b);
   if (absa > absb)
    return absa*sqrt(1.0+SQR(absb/absa));
  else
    return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}


int TSVDcalc::svdGetUWV(fd_type *A, fd_type w[], fd_type *V)
// Given a matrix a[1..m][1..n], this routine computes its singular value decomposition, A =
// UÂ·WÂ·V T. Thematrix U replaces a on output. The diagonal matrix of singular values W is output
// as a vector w[1..n]. Thematrix V (not the transpose V T ) is output as v[1..n][1..n].
{
  int flag,i,its,j,jj,k,l,nm;
  fd_type anorm,c,f,g,h,s,scale,x,y,z,*rv1, *aa;

  rv1= new fd_type[n]; //vector(1,n);
  aa = new fd_type[m]; //vector(1,n);
  g=scale=anorm=0.0; // Householder reduction to bidiagonal form.
  for (i=0;i<n;i++)
  {
     l=i+1;
     rv1[i]=scale*g;
     g=s=scale=0.0;
     if (i < m)
     {
       for (k=i;k<m;k++)
          scale += fabs(a(k,i));
       if (scale)
       {
         for (k=i;k<m;k++)
         {
           a(k,i) /= scale;
           s += a(k, i)*a(k,i);
         }
        f = a(i,i);
        g = -SIGN(sqrt(s),f);
        h = f*g-s;
        a(i,i)=f-g;
        for (j=l;j<n;j++)
        {
            for (s=0.0,k=i;k<m;k++)
              s += a(k,i)*a(k,j);
            f=s/h;
            for (k=i;k<m;k++)
              a(k,j) += f*a(k,i);
        }
        for (k=i;k<m;k++) a(k,i) *= scale;
     }
    }
    w[i]=scale *g;
    g = s = scale = 0.0;
    if (i < m && i != (n-1))
    {
       for (k=l;k<n;k++)
        scale += fabs(a(i,k));
       if (scale)
       {
          for (k=l;k<n;k++)
          {
            a(i,k) /= scale;
            s += a(i,k)*a(i,k);
          }
          f=a(i,l);
          g = -SIGN(sqrt(s),f);
          h=f*g-s;
          a(i,l) = f-g;
          for (k=l;k<n;k++)
             rv1[k]=a(i,k)/h;
          for (j=l;j<m;j++)
          {
            for (s=0.0,k=l;k<n;k++)
              s += a(j,k)*a(i,k);
            for (k=l;k<n;k++)
              a(j,k) += s*rv1[k];
          }
          for (k=l;k<n;k++)
            a(i,k) *= scale;
      }
    }
    anorm = FMAX(anorm,(fabs(w[i])+fabs(rv1[i])));
  }
  for (i=n-1;i>=0;i--) // Accumulation of right-hand transformations.
  {
     if (i < (n-1))
     {
       if (g)
       {
           for (j=l;j<n;j++) // Double division to avoid possible underflow.
                 v(j,i)=(a(i,j)/a(i,l))/g;
           for (j=l;j<n;j++)
           {
              for (s=0.0,k=l;k<n;k++)
                 s += a(i,k)*v(k,j);
              for (k=l;k<n;k++)
                 v(k,j) += s*v(k,i);
           }
       }
       for (j=l;j<n;j++)
         v(i,j)=v(j,i)=0.0;
     }
     v(i,i)=1.0;
     g=rv1[i];
     l=i;
   }
   for(i = IMIN((m-1),(n-1));i>=0;i--)  // Accumulation of left-hand transformations.
   {
     l=i+1;
     g=w[i];
     for (j=l;j<n;j++)
       a(i,j)=0.0;
     if (g)
     {
        g=1.0/g;
        for(j=l;j<n;j++)
        {
           for (s=0.0,k=l;k<m;k++)
             s += a(k,i)*a(k,j);
           f=(s/a(i,i))*g;
           for (k=i;k<m;k++)
             a(k,j) += f*a(k,i);
        }
       for (j=i;j<m;j++)
         a(j,i) *= g;
    }
    else
       for (j=i;j<m;j++)
           a(j,i)=0.0;
    ++a(i,i);
   }
   for (k=(n-1);k>=0;k--)  // Diagonalization of the bidiagonal form: Loop over
   {                       //singular values, and over allowed iterationsfor
       for(its=1;its<=30;its++)
       {
           flag=1;
           for (l=k;l>=0;l--)  //Test for splitting.
           {                  // Note that rv1[1] is always zero.
              nm=l-1;
              if ((fd_type)(fabs(rv1[l])+anorm) == anorm)
              {
                 flag=0;
                 break;
              }
              if(nm >= 0 ) //SD
                if ((fd_type)(fabs(w[nm])+anorm) == anorm)
                 break;
          }
          if (flag)
          {
            c=0.0; // Cancellation of rv1[l], if l > 1.
            s=1.0;
            for (i=l;i<=k;i++)
            {
             f=s*rv1[i];
             rv1[i]=c*rv1[i];
             if ((fd_type)(fabs(f)+anorm) == anorm)
               break;
             g=w[i];
             h=pyt_hag(f,g);
             w[i]=h;
              h=1.0/h;
             c=g*h;
             s = -f*h;
             for (j=0;j<m;j++)
             {
               if(nm >= 0 ) //SD
                y=a(j,nm);
               else y = aa[j];  //SD
               z=a(j,i);
               if(nm >= 0 ) //SD
                 a(j,nm)=y*c+z*s;
                else aa[j]= y*c+z*s;  //SD
               a(j,i)=z*c-y*s;
             }
            }
          }
          z=w[k];
          if (l == k)   // Convergence.
          {
             if (z < 0.0)  // Singular value is made nonnegative.
             {
                w[k] = -z;
                for (j=0;j<n;j++)
                  v(j,k) = -v(j,k);
             }
            break;
          }
//         if (its == 30)
//            return -1; //nrerror("no convergence in 30 svdcmp iterations");
         ErrorIf (its == 30, "GetUWV","no convergence in 30 iterations" );
         x=w[l]; // Shift from bottom 2-by-2 minor.
         nm=k-1;
         if(nm >= 0 ) //SD
         { y=w[nm];
           g=rv1[nm];
         }else y = g =0; //SD
         h=rv1[k];
         f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
         g=pyt_hag(f,1.0);
         f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
         c=s=1.0; // Next QR transformation:
         for (j=l;j<=nm;j++)
         {
             i=j+1;
             g=rv1[i];
             y=w[i];
             h=s*g;
             g=c*g;
             z=pyt_hag(f,h);
             rv1[j]=z;
             c=f/z;
             s=h/z;
             f=x*c+g*s;
             g = g*c-x*s;
             h=y*s;
             y *= c;
             for (jj=0;jj<n;jj++)
             {
               x=v(jj,j);
               z=v(jj,i);
               v(jj,j)=x*c+z*s;
               v(jj,i)=z*c-x*s;
             }
             z=pyt_hag(f,h);
             w[j]=z; // Rotation can be arbitrary if z = 0.
             if (z)
             {
               z=1.0/z;
               c=f*z;
               s=h*z;
              }
             f=c*g+s*y;
             x=c*y-s*g;
             for (jj=0;jj<m;jj++)
             {
                 y=a(jj,j);
                 z=a(jj,i);
                 a(jj,j)=y*c+z*s;
                 a(jj,i)=z*c-y*s;
             }
         }
         rv1[l]=0.0;
         rv1[k]=f;
         w[k]=x;
      }
   }

 delete[] rv1; //free_vector(rv1,1,n);
 delete[] aa; //free_vector(rv1,1,n);
 return 1;
}


// Chapter 15.676

void TSVDcalc::svdMin(  fd_type a[],
     fd_type *U, fd_type *V,  fd_type w[],  fd_type& chisq )
// Given a set of data points x[1..ndata],y[1..ndata] with individual coeff
// wpar[1..ndata], use ƒÔ2 minimization to determine the coefficients a[1..ma]
// of the fitting  function y = Sumi (ai*afunci(x)).
// Here we solve the fitting equations.
// Arrays u[1..ndata][1..ma], v[1..ma][1..ma],
// and w[1..ma] provide workspace on input; on output they define the
// singular value decomposition, and can be used to obtain the covariance matrix.
// The program  returns values for the ma fit parameters a, and X, chisq.
// The user supplies a routine funcs(x,afunc,ma) that returns the ma basis
// functions evaluated at x = x in the array afunc[1..ma].
{

   fd_type wmax,tmp,thresh,sum,*b,*afunc;
   int i, j;

   b = new fd_type[m];  //vector(1,ndata);
   afunc = new fd_type[n]; //vector(1,ma);

   for (i=0;i<m;i++)  // Accumulate coefficients of the fitting matrix
   {
       data->par_funct( i, data->getX(i),afunc );
       tmp= data->getWdat(i); // 1.0/sig[i];
       for (j=0;j<n;j++)
         u(i,j) = afunc[j]*tmp;
       b[i] = data->getY(i) * tmp;
    }

    svdGetUWV(U,w,V);    //Singular value decomposition.

    wmax=0.0;                     // Edit the singular values, given TOL from
    for (j=0;j<n;j++)
       if (w[j] > wmax) wmax=w[j];
    thresh=TOL*wmax;
    for (j=0;j<n;j++)
       if (w[j] < thresh) w[j]=0.0;

    svdGetX(U,w,V,b,a);

    chisq=0.0;                    // Evaluate chi-square.
    for (i=0;i<m;i++)
    {
       data->par_funct( i, data->getX(i),afunc );
       for (sum=0.0,j=0;j<n;j++)
         sum += a[j]*afunc[j];
       tmp=(data->getY(i)-sum)*data->getWdat(i);
       chisq += (tmp,tmp*tmp);
   }
   chisq = sqrt(chisq);
   data->xi2 = chisq;

   delete[] afunc;
   delete[] b;
}

void TSVDcalc::svdStat(fd_type *V, fd_type w[], fd_type *CVM)
// To evaluate the covariance matrix cvm[1..ma][1..ma]
// of the fit for ma parameters obtained  by svdfit,
// call this routine with matrices v[1..ma][1..ma], w[1..ma]
// as returned from svdfit.
{
   int k,j,i;
   fd_type sum,*wti;

   wti = new fd_type[n]; // vector(1,ma);
   for(i=0;i<n;i++)
   {
       wti[i]=0.0;
       if(w[i])
         wti[i]=1.0/(w[i]*w[i]);
   }
   for(i=0;i<n;i++) // Sum contributions to covariance matrix
     for(j=0;j<=i;j++)
     {
       for (sum=0.0,k=0;k<n;k++)
          sum += v(i,k)*v(j,k)*wti[k];
       cvm(j,i) = cvm(i,j) = sum;
     }

   delete[] wti; //  free_vector(wti,1,ma);
}






