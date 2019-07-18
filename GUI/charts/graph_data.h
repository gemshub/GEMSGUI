//  This is JSONUI library+API (https://bitbucket.org/gems4/jsonui)
//
/// \file graph_data.h
/// Declarations of  classes SeriesLineData and ChartData
/// internal representation of graphic data
/// (Plotting system description)
//
// JSONUI is a C++ Qt5-based widget library and API aimed at implementing 
// the GUI for editing/viewing the structured data kept in a NoSQL database,
// with rendering documents to/from JSON/YAML/XML files, and importing/
// exporting data from/to arbitrary foreign formats (csv etc.). Graphical 
// presentation of tabular data (csv format fields) is also possible.
//
// Copyright (c) 2017 Svetlana Dmytriieva (svd@ciklum.com) and
//   Dmitrii Kulik (dmitrii.kulik@psi.ch)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU (Lesser) General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// JSONUI depends on the following open-source software products:
// JSONIO (https://bitbucket.org/gems4/jsonio); Qt5 (https://qt.io).
//

#ifndef graph_data_h
#define graph_data_h

#include <memory>
#include <QObject>
#include <QFont>
#include <QColor>
#include <QJsonObject>
#include "chart_model.h"

namespace jsonui {


enum GRAPHTYPES_ {
                 LineChart = 0,
                 AreaChart = 1,
                 BarChart  = 2,
                 Isolines  = 3,   // under construction
                 lines_3D  = 4    // for future using
};

class SeriesLineData;

QImage markerShapeImage( const SeriesLineData& linedata );
QIcon markerShapeIcon( const SeriesLineData& linedata );
QImage textImage( const QFont& font, const QString& text );

//QImage paintMarkerShape( const SeriesLineData& linedata, bool onLegend =false );
void getLinePen( QPen& pen, const SeriesLineData& linedata  );

QColor colorAt(const QColor &start, const QColor &end, qreal pos);

/// Description of one plot curve -
/// the representation of a series of points in the x-y plane
class SeriesLineData
{
    int markerShape;  ///< Type of points (old pointType)
    int markerSize;   ///< Size of points (old pointSize)
    int penSize;      ///< Size of lines  (old lineSize )
    int penStyle;     ///< Style of lines ( enum Qt::PenStyle )
    int spline;       ///< Use Spline chart

    // point color  - the Constructs a color with the RGB values
    int red;    ///< Constructs a color with the RGB value red
    int green;  ///< Constructs a color with the RGB value green
    int blue;   ///< Constructs a color with the RGB value blue

    std::string name;  ///< This property holds the name of the series
    int xcolumn;       ///< This property holds the column of the model
                       ///< that contains the x-coordinates of data points ( old ndxX )

public:

    SeriesLineData( const std::string& aName = "",
               int mrkType = 0, int mrkSize = 8,
               int lineSize = 2,  int lineStyle = 1, int usespline =0,
               const QColor& aColor = QColor( 25, 0, 150)  ):
        name(aName), xcolumn(-1) // iterate by index
    {
       setChanges( mrkType, mrkSize, lineSize,  lineStyle, usespline, aColor );
    }

    SeriesLineData( size_t ndx, size_t maxLines, const std::string& aName = "",
               int /*mrkType*/ = 0, int mrkSize = 8,
               int lineSize = 2,  int lineStyle = 1, int usespline =0 ):
        name(aName), xcolumn(-1)
    {
        QColor aColor;
        aColor.setHsv( 360/maxLines*ndx, 200, 200);
        //aColor = colorAt(green, blue, double(ndx)/maxLines );
        setChanges( ndx%20/*mrkType*/, mrkSize, lineSize,  lineStyle, usespline, aColor );
    }

    int getMarkerShape() const
    {
        return markerShape;
    }

    void setMarkerShape( int aType )
    {
        markerShape = aType;
    }

    int getMarkerSize() const
    {
        return markerSize;
    }

    int getPenSize() const
    {
        return penSize;
    }

    int getPenStyle() const
    {
        return penStyle;
    }

    int getSpline() const
    {
        return spline;
    }

    QColor getColor() const
    {
        return QColor(red, green, blue);
    }

    void setChanges( int mrkType, int mrkSize, int pnSize,
                     int pnStyle, int usespline, const QColor& aColor )
    {
        markerShape = mrkType;
        markerSize = mrkSize;
        penSize = pnSize;
        penStyle = pnStyle;
        spline  = usespline;
        red =   aColor.red();
        green = aColor.green();
        blue  = aColor.blue();
    }

    void setName( const std::string& aName )
    {
       name = aName;
    }

    const std::string& getName() const
    {
        return name;
    }

    int getXColumn() const
    {
        return xcolumn;
    }

    void setXColumn( int aNdxX )
    {
       xcolumn = aNdxX;
    }

#ifndef NO_JSONIO
    void toJsonNode( jsonio::JsonDom *object ) const;
    void fromJsonNode( const jsonio::JsonDom *object );
#endif

    void toJsonObject(QJsonObject& json) const;
    void fromJsonObject(const QJsonObject& json);

};


/// Description of 2D plotting widget
class ChartData : public QObject
{
    Q_OBJECT

 public slots:

   void updateXSelections();
   void updateYSelections( bool updateNames );

 public:

    std::string title;  ///< Title of graphic
    int graphType;      ///< GRAPHTYPES ( 0-line by line, 1- cumulative, 2 - isolines )

    // define grid of plot
    int axisTypeX;      ///< Grid type for Abscissa
    int axisTypeY;      ///< Grid type for Ordinate
    std::string xName;  ///< Abscissa name
    std::string yName;  ///< Ordinate name
    double region[4];   ///< Graph Min Max Region
    double part[4];     ///< Fragment Min Max Region

    /// Define background color ( the Constructs a color with the RGB values)
    int b_color[3]; // red, green, blue

    /// Define Axis Font
    QFont axisFont;

//------------------------------------------------------------

    template <class T>
      ChartData( const std::vector<std::shared_ptr<T>>& aPlots,  const std::string& atitle,
               const std::string& aXName, const std::string& aYname,
               int agraphType = LineChart ):
          title(atitle), graphType( agraphType ), axisTypeX(5), axisTypeY(5),
          xName(aXName), yName(aYname), axisFont("Sans Serif", 10)
      {
        // Define background color
        setBackgroundColor( QColor(Qt::white) );

        // Insert Plots and curves description
        modelsdata.clear();
        for( auto plot: aPlots)
          addNewPlot( plot );

        // Graph&Fragment Min Max Region
        double regg[4] = {0., 0., 0., 0.};
        setMinMaxRegion( regg );
      }


    ~ChartData()
    {}

    /// add new plot lines selection
    template <class T>
       void addNewPlot( const std::shared_ptr<T>& aPlot )
    {
       int defined_lines = static_cast<int>(linesdata.size());
       int nLines = getSeriesNumber();

       modelsdata.push_back( aPlot );
       int nLinN = aPlot->getSeriesNumber();
       for( int jj=0; jj<nLinN; jj++, nLines++ )
       {
         if( nLines >= defined_lines )
           linesdata.push_back( SeriesLineData( jj, nLinN, aPlot->getName(nLinN)  ) );
       }

       connect( modelsdata.back().get(), SIGNAL( changedXSelections() ),
                this,  SLOT( updateXSelections() ) );
       connect( modelsdata.back().get(), SIGNAL( changedYSelections(bool) ),
                this,  SLOT( updateYSelections(bool) ) );
     }


    /// get plot from index
    int getPlot( size_t line, size_t* modelline=nullptr ) const
    {
        size_t sizecnt=0;
        for( size_t ii=0 ; ii<modelsdata.size(); ii++)
        {
           sizecnt += modelsdata[ii]->getSeriesNumber();
           if( line < sizecnt )
           {
               if( modelline )
                   *modelline = line - sizecnt + modelsdata[ii]->getSeriesNumber();
               return static_cast<int>(ii);
           }
         }
        return -1;
    }

    /// Get number of series
    int getSeriesNumber() const
    {
      int nmb = 0;
      for( auto model: modelsdata)
        nmb += model->getSeriesNumber();
      return nmb;
    }

   size_t modelsNumber() const
   {
     return  modelsdata.size();
   }

   ChartDataModel* modelData( size_t ndx )
   {
     return  modelsdata[ndx].get();
   }

   size_t linesNumber() const
   {
     return  linesdata.size();
   }

   const SeriesLineData& lineData( size_t ndx ) const
   {
     return  linesdata[ndx];
   }

   void setLineData( size_t ndx, const SeriesLineData& newData  )
   {
     linesdata[ndx] = newData;
   }

   void setLineData( size_t ndx,  const QString aNdxX  )
   {
     int modelndx = getPlot( ndx);
     if( modelndx >= 0)
       linesdata[ndx].setXColumn( modelsdata[ static_cast<size_t>(modelndx)]->indexAbscissaName( aNdxX ) );
   }

   void setLineData( size_t ndx,  const std::string& aName  )
   {
     linesdata[ndx].setName( aName );
   }

   QColor getBackgroundColor() const
   {
       return QColor(b_color[0], b_color[1], b_color[2]);
   }

   void setBackgroundColor( const QColor& aColor )
   {
       b_color[0] =   aColor.red();
       b_color[1] = aColor.green();
       b_color[2]  = aColor.blue();
   }

   void setMinMaxRegion( double reg[4] );

   void setAxisTypes( int axisX, int axisY )
   {
       axisTypeX = axisX;
       axisTypeY = axisY;
   }

#ifndef NO_JSONIO
    void toJsonNode( jsonio::JsonDom *object ) const;
    void fromJsonNode( const jsonio::JsonDom *object );
#endif

   void toJsonObject(QJsonObject& json) const;
   void fromJsonObject(const QJsonObject& json);

protected:

   // define curves
   std::vector<std::shared_ptr<ChartDataModel>> modelsdata;   ///< Descriptions of model extracting data
   std::vector<SeriesLineData> linesdata;     ///< Descriptions of all lines

   ChartData( const ChartData& data );   // not defined
   ChartData& operator=(const ChartData&); // not defined

};

} // namespace jsonui

#endif   // _graph_data_h

