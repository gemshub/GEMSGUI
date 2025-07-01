
#        CONFIG   -= qt
        
        HEADERS	 += $$MODULES_H/filters_data.h \
                    $$MODULES_H/m_compos.h  \
                    $$MODULES_H/m_const.h \
                    $$MODULES_H/m_dcomp.h \
		    $$MODULES_H/m_dualth.h \
		    $$MODULES_H/m_gem2mt.h \
		    $$MODULES_H/m_gtdemo.h \
                    $$MODULES_H/m_icomp.h  \
                    $$MODULES_H/m_param.h  \
                    $$MODULES_H/m_phase.h  \
                    $$MODULES_H/m_proces.h  \
                    $$MODULES_H/m_reacdc.h  \
                    $$MODULES_H/m_rtparm.h  \
                    $$MODULES_H/m_sdata.h  \
                    $$MODULES_H/m_syseq.h  \
                    $$MODULES_H/m_unspace.h  \
                    $$MODULES_H/s_formula.h  \
                    $$MODULES_H/s_lsm.h  \
                    $$MODULES_H/s_supcrt.h  \
                    $$MODULES_H/s_tpwork.h  \
                    $$MODULES_H/v_ipnc.h  \
                    $$MODULES_H/v_mod.h   \
                    $$MODULES_H/v_thermofun.h \
                    $$MODULES_H/gemsgui_version.h

        SOURCES	 +=  $$MODULES_CPP/m_const.cpp \
                     $$MODULES_CPP/m_sdata.cpp \
                     $$MODULES_CPP/m_gem2mt.cpp \
                     $$MODULES_CPP/m_gem2mtc.cpp \
                     $$MODULES_CPP/m_gem2mtt.cpp \
                     $$MODULES_CPP/m_gem2mtbox.cpp \
                     $$MODULES_CPP/m_gem2mtfor.cpp \
                     $$MODULES_CPP/m_param.cpp \
                     $$MODULES_CPP/m_icomp.cpp \
                     $$MODULES_CPP/v_ipnc.cpp \
                     $$MODULES_CPP/s_formula.cpp \
                     $$MODULES_CPP/m_dcomp.cpp \
                     $$MODULES_CPP/m_dcomp2.cpp \
                     $$MODULES_CPP/s_tpwork.cpp \
                     $$MODULES_CPP/s_supcrt.cpp \
                     $$MODULES_CPP/s_supcrt2.cpp \
                     $$MODULES_CPP/m_compos.cpp \
                     $$MODULES_CPP/s_lsm_fun.cpp \
                     $$MODULES_CPP/s_lsm_lm.cpp  \
                     $$MODULES_CPP/s_lsm_svd.cpp \
                     $$MODULES_CPP/m_reacdc.cpp  \
                     $$MODULES_CPP/m_reac2.cpp  \
                     $$MODULES_CPP/m_phase.cpp \
                     $$MODULES_CPP/m_phase2.cpp \
                     $$MODULES_CPP/m_proces.cpp \
                     $$MODULES_CPP/m_prfget.cpp \
                     $$MODULES_CPP/m_prfget2.cpp \
                     $$MODULES_CPP/m_syseq.cpp \
                     $$MODULES_CPP/m_syst2.cpp  \
                     $$MODULES_CPP/m_unspace.cpp \
                     $$MODULES_CPP/m_unsp_gen.cpp \
                     $$MODULES_CPP/m_unsp_stat.cpp \
                     $$MODULES_CPP/m_unsp_out.cpp \
                     $$MODULES_CPP/m_dualth.cpp  \
                     $$MODULES_CPP/m_dualthc.cpp  \
                     $$MODULES_CPP/m_gtdemo.cpp   \
                     $$MODULES_CPP/m_rtparm.cpp \
                     $$MODULES_CPP/v_thermofun.cpp
      
