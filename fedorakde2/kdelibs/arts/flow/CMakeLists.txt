
add_custom_command(
    OUTPUT artsflow.h artsflow.cc
    COMMAND mcopidl ${CMAKE_CURRENT_SOURCE_DIR}/artsflow.idl
    )

set(artsflow_source
        asyncschedule.cc
        audioio.cc

        audiomanager_impl.cc
        audiosubsys.cc
        bus.cc
        bytestreamtoaudio_impl.cc
        cache.cc
        convert.cc
        cpuinfo.cc
        fft.c
        pipebuffer.cc
        resample.cc
        stdsynthmodule.cc
        stereoeffectstack_impl.cc
        stereofftscope_impl.cc
        stereovolumecontrol_impl.cc
        synth_add_impl.cc
        synth_frequency_impl.cc
        synth_mul_impl.cc
        synth_multi_add_impl.cc
        synth_play_impl.cc
        synth_play_wav_impl.cc
        synth_record_impl.cc
        synth_wave_sin_impl.cc
        synthschedule.cc
        virtualports.cc

        audioionull.cc
    )

if(ENABLE_NAS)
    set(artsflow_source audioionas.cc ${artsflow_source})
endif()

if(ENABLE_AUDIO_SUN)
    set(artsflow_source audioiosun.cc ${artsflow_source})
endif()

if(ENABLE_AUDIO_AIX)
    set(artsflow_source audioioaix.cc ${artsflow_source})
endif()
if(ENABLE_AUDIOIO)
    set(artsflow_source audioiolibaudioio.cc ${artsflow_source})
endif()

if(ENABLE_OSS)
    set(artsflow_source audioiooss.cc ${artsflow_source})
endif()

if(ENABLE_ALSA)
    set(artsflow_source audioioalsa9.cc audioioalsa.cc ${artsflow_source})
endif()

kde2_library(libartsflow
    SOURCES
        artsflow.cc
        ${artsflow_source}
    HEADERS
        audiosubsys.h
        cache.h
        cachedwav.h
        convert.h
        pipebuffer.h
        stdsynthmodule.h
        synthschedule.h
        fft.h
        audioio.h
        resample.h
        cpuinfo.h
        artsflow.idl
    INCLUDE_DIRECTORIES
        ${ALSA_INCLUDE_DIRS}
    LIBS
        kde2::mcop
        ${ALSA_LIBRARY}
    VERSION
        3.0.0
    )
target_link_libraries(libartsflow PRIVATE PkgConfig::audiofile)
target_compile_definitions(libartsflow PUBLIC -DALSA_PCM_OLD_SW_PARAMS_API -DALSA_PCM_OLD_HW_PARAMS_API)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/artsflow.h DESTINATION ${KDE2_INCLUDEDIR}/artsflow/)
