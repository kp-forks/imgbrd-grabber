set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) # Don't delete TS files

find_package(Qt6 COMPONENTS LinguistTools REQUIRED)

# Get two separate lists of TS files: one with only 'YourLanguage', and another with all other translations
set(TS_FILES_FOR_TS "${CMAKE_CURRENT_SOURCE_DIR}/YourLanguage.ts")
file(GLOB TS_FILES_FOR_QM "*.ts")
include(ListFilterRegex)
listFilterRegex(TS_FILES_FOR_QM "YourLanguage.ts$")

qt_create_translation(QM_FILES_FOR_TS ${SOURCES} ${TS_FILES_FOR_TS}) # Re-generate TS and generate QM for 'YourLanguage'
qt_add_translation(QM_FILES ${TS_FILES_FOR_QM}) # Only generate QM for actual translations

add_custom_target(CrashReporter_translations ALL DEPENDS ${QM_FILES_FOR_TS} ${QM_FILES})

if(APPLE AND UNIX)
	install(FILES ${QM_FILES} DESTINATION Grabber.app/Contents/Resources/crashreporter)
else()
	install(FILES ${QM_FILES} DESTINATION share/Grabber/crashreporter)
endif()
