target_sources(${PROJECT_NAME} PRIVATE
    UBExportAdaptor.cpp
    UBExportAdaptor.h
    UBExportDocument.cpp
    UBExportDocument.h
    UBExportDocumentSetAdaptor.cpp
    UBExportDocumentSetAdaptor.h
    UBExportFullPDF.cpp
    UBExportFullPDF.h
    UBExportPDF.cpp
    UBExportPDF.h
    UBExportWeb.cpp
    UBExportWeb.h
    UBImportAdaptor.cpp
    UBImportAdaptor.h
    UBImportDocument.cpp
    UBImportDocument.h
    UBImportDocumentSetAdaptor.cpp
    UBImportDocumentSetAdaptor.h
    UBImportImage.cpp
    UBImportImage.h
    UBImportPDF.cpp
    UBImportPDF.h
    UBMetadataDcSubsetAdaptor.cpp
    UBMetadataDcSubsetAdaptor.h
    UBPageMapper.cpp
    UBPageMapper.h
    UBSvgSubsetAdaptor.cpp
    UBSvgSubsetAdaptor.h
    UBThumbnailAdaptor.cpp
    UBThumbnailAdaptor.h
    UBWidgetUpgradeAdaptor.cpp
    UBWidgetUpgradeAdaptor.h
)

get_property(OUTVAR DIRECTORY PROPERTY COMPILE_DEFINITIONS)
if(ENABLE_SHAPES IN_LIST OUTVAR)
    target_sources(openboard PRIVATE
        UBSvgShapeAdaptor.cpp
        UBSvgShapeAdaptor.h
    )
endif()
