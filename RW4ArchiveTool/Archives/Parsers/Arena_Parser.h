// EA Skate Arena Parser By GHFear.
#pragma once
#include <filesystem>
#include "IoTools.h"
#include "../Structs/SF_Structs.h"
#include "../../Tools/Paths/Paths.h"
#include "../Tools/big_endian_tools.h"

namespace arena
{
    std::string ArenaObjectTypeNames[] {
        "RWOBJECTTYPE_NA",
        "RWOBJECTTYPE_ARENA",
        "RWOBJECTTYPE_RAW",
        "RWOBJECTTYPE_SUBREFERENCE",
        "RWOBJECTTYPE_SECTIONMANIFEST",
        "RWOBJECTTYPE_SECTIONTYPES",
        "RWOBJECTTYPE_SECTIONEXTERNALARENAS",
        "RWOBJECTTYPE_SECTIONSUBREFERENCES",
        "RWOBJECTTYPE_SECTIONATOMS",
        "RWOBJECTTYPE_DEFARENAIMPORTS",
        "RWOBJECTTYPE_DEFARENAEXPORTS",
        "RWOBJECTTYPE_DEFARENATYPES",
        "RWOBJECTTYPE_DEFARENADEFINEDARENAID",
        "RWOBJECTTYPE_ATTRIBUTEPACKET",
        "RWOBJECTTYPE_ATTRIBUTEPACKET_DELEGATE",
        "RWOBJECTTYPE_BITTABLE",
        "RWOBJECTTYPE_ARENALOCALATOMTABLE",
        "RWOBJECTTYPE_BASERESOURCE_START",
        "RWOBJECTTYPE_BASERESOURCE_1",
        "RWOBJECTTYPE_BASERESOURCE_2",
        "RWOBJECTTYPE_BASERESOURCE_3",
        "RWOBJECTTYPE_BASERESOURCE_4",
        "RWOBJECTTYPE_BASERESOURCE_5",
        "RWOBJECTTYPE_BASERESOURCE_6",
        "RWOBJECTTYPE_BASERESOURCE_7",
        "RWOBJECTTYPE_BASERESOURCE_8",
        "RWOBJECTTYPE_BASERESOURCE_9",
        "RWOBJECTTYPE_BASERESOURCE_A",
        "RWOBJECTTYPE_BASERESOURCE_B",
        "RWOBJECTTYPE_BASERESOURCE_C",
        "RWOBJECTTYPE_BASERESOURCE_D",
        "RWOBJECTTYPE_BASERESOURCE_E",
        "RWOBJECTTYPE_BASERESOURCE_RESERVEDTO",
        "RWGOBJECTTYPE_NA",
        "RWGOBJECTTYPE_CAMERA",
        "RWGOBJECTTYPE_PALETTE",
        "RWGOBJECTTYPE_RASTER",
        "RWGOBJECTTYPE_VDES",
        "RWGOBJECTTYPE_VBUF",
        "RWGOBJECTTYPE_IDES",
        "RWGOBJECTTYPE_IBUF",
        "RWGOBJECTTYPE_LIGHT",
        "RWGOBJECTTYPE_MESH",
        "RWGOBJECTTYPE_SHADER",
        "RWGOBJECTTYPE_COMPILEDSTATE",
        "RWGOBJECTTYPE_RENDEROBJECT",
        "RWGOBJECTTYPE_GSDATA",
        "RWGOBJECTTYPE_VERTEXSHADER",
        "RWGOBJECTTYPE_VERTEXDATA",
        "RWGOBJECTTYPE_INDEXDATA",
        "RWGOBJECTTYPE_RASTERDATA",
        "RWGOBJECTTYPE_BUILDSTATE",
        "RWGOBJECTTYPE_PIXELSHADER",
        "RWGOBJECTTYPE_PROFILEMESH",
        "RWGOBJECTTYPE_DESIGNVIEWOBJECT",
        "RWGOBJECTTYPE_PROFILERENDEROBJECT",
        "RWGOBJECTTYPE_IMAGE",
        "RWGOBJECTTYPE_RENDEROBJECTCONTAINER",
        "RWGOBJECTTYPE_MESHCOMPILEDSTATELINK",
        "RWGOBJECTTYPE_SHADERCODE",
        "RWGOBJECTTYPE_FONT",
        "RWGOBJECTTYPE_GLYPHTABLE",
        "RWGOBJECTTYPE_KERNTABLE",
        "RWGOBJECTTYPE_PAGETABLE",
        "RWGOBJECTTYPE_RASTERTEXTURE",
        "RWGOBJECTTYPE_FACENAME",
        "RWGOBJECTTYPE_UPDATELOCATOR",
        "RWGOBJECTTYPE_MESHHELPER",
        "RWGOBJECTTYPE_NA1",
        "RWGOBJECTTYPE_VIEWPORTPARAMETERS",
        "RWGOBJECTTYPE_SCISSORRECTPARAMETERS",
        "RWGOBJECTTYPE_CLEARCOLORPARAMETERS",
        "RWGOBJECTTYPE_CLEARDEPTHSTENCILPARAMETERS",
        "RWGOBJECTTYPE_DRAWINDEXEDPARAMETERS",
        "RWGOBJECTTYPE_DRAWPARAMETERS",
        "RWGOBJECTTYPE_PIXELBUFFER",
        "RWGOBJECTTYPE_TEXTURE",
        "RWGOBJECTTYPE_VERTEXDESCRIPTOR",
        "RWGOBJECTTYPE_VERTEXBUFFER",
        "RWGOBJECTTYPE_INDEXBUFFER",
        "RWGOBJECTTYPE_PROGRAMBUFFER",
        "RWGOBJECTTYPE_STATEBUFFER",
        "RWGOBJECTTYPE_PROGRAMSTATEBUFFER",
        "RWGOBJECTTYPE_PROGRAMSTATES",
        "RWGOBJECTTYPE_SAMPLERSTATE",
        "RWGOBJECTTYPE_RENDERTARGETSTATE",
        "RWGOBJECTTYPE_BLENDSTATE",
        "RWGOBJECTTYPE_DEPTHSTENCILSTATE",
        "RWGOBJECTTYPE_RASTERIZERSTATE",
        "RWGOBJECTTYPE_VERTEXPROGRAMSTATE",
        "RWGOBJECTTYPE_PROGRAMSTATEHANDLE",
        "RWGOBJECTTYPE_DRAWINSTANCEDPARAMETERS",
        "RWGOBJECTTYPE_DRAWINDEXEDINSTANCEDPARAMETERS",
        "OBJECTTYPE_NA1",
        "OBJECTTYPE_PART",
        "OBJECTTYPE_PARTDEFINITION",
        "OBJECTTYPE_JOINTEDPAIR",
        "OBJECTTYPE_JOINTLIMIT",
        "OBJECTTYPE_JOINTSKELETON",
        "OBJECTTYPE_ASSEMBLY",
        "OBJECTTYPE_ASSEMBLYDEFINITION",
        "OBJECTTYPE_NA2",
        "OBJECTTYPE_KEYFRAMEANIM",
        "OBJECTTYPE_SKELETON",
        "OBJECTTYPE_ANIMATIONSKIN",
        "OBJECTTYPE_INTERPOLATOR",
        "OBJECTTYPE_FEATHERINTERPOLATOR",
        "OBJECTTYPE_ONEBONEIK",
        "OBJECTTYPE_TWOBONEIK",
        "OBJECTTYPE_BLENDER",
        "OBJECTTYPE_WEIGHTEDBLENDER",
        "OBJECTTYPE_REMAPPER",
        "OBJECTTYPE_SKELETONSINK",
        "OBJECTTYPE_SKINSINK",
        "OBJECTTYPE_LIGHTSINK",
        "OBJECTTYPE_CAMERASINK",
        "OBJECTTYPE_SKINMATRIXBUFFER",
        "OBJECTTYPE_TWEAKCONTROLLER",
        "OBJECTTYPE_SHADERSINK",
        "RWCOBJECTTYPE_NA2",
        "RWCOBJECTTYPE_VOLUME",
        "RWCOBJECTTYPE_SIMPLEMAPPEDARRAY",
        "RWCOBJECTTYPE_TRIANGLEKDTREEPROCEDURAL",
        "RWCOBJECTTYPE_KDTREEMAPPEDARRAY",
        "RWCOBJECTTYPE_BBOX",
        "RWCOBJECTTYPE_CLUSTEREDMESH",
        "RWOBJECTTYPE_RENDERMESHDATA",
        "RWOBJECTTYPE_RENDERMODELDATA",
        "RWOBJECTTYPE_SIMPLETRIMESHDATA",
        "RWOBJECTTYPE_SPLINEDATA",
        "RWOBJECTTYPE_RENDERMATERIALDATA",
        "RWOBJECTTYPE_COLLISIONMATERIALDATA",
        "RWOBJECTTYPE_ROLLERDESCDATA",
        "RWOBJECTTYPE_VERSIONDATA",
        "RWOBJECTTYPE_LOCATIONDESCDATA",
        "RWOBJECTTYPE_COLLISIONMODELDATA",
        "RWOBJECTTYPE_TABLEOFCONTENTS",
        "RWOBJECTTYPE_COLLISIONBEZIERDATA",
        "RWOBJECTTYPE_INSTANCEDATA",
        "RWOBJECTTYPE_RENDERBLENDSHAPEEDATA",
        "RWOBJECTTYPE_WORLDPAINTERLAYERDATA",
        "RWOBJECTTYPE_WORLDPAINTERQUADTREEDATA",
        "RWOBJECTTYPE_WORLDPAINTERDICTIONARYDATA",
        "RWOBJECTTYPE_NAVMESHDATA",
        "RWOBJECTTYPE_RAINDATA",
        "RWOBJECTTYPE_AIPATHDATA",
        "RWOBJECTTYPE_STATSDATA",
        "RWOBJECTTYPE_MASSIVEDATA",
        "RWOBJECTTYPE_DEPTHMAPDATA",
        "RWOBJECTTYPE_LIONDATA",
        "RWOBJECTTYPE_TRIGGERINSTANCEDATA",
        "RWOBJECTTYPE_WAYPOINTDATA",
        "RWOBJECTTYPE_EMBEDDEDDATA",
        "RWOBJECTTYPE_EMITTERWAYPOINTDATA",
        "RWOBJECTTYPE_DMODATA",
        "RWOBJECTTYPE_HOTPOINTDATA",
        "RWOBJECTTYPE_GRABDATA",
        "RWOBJECTTYPE_SPATIALMAP",
        "RWOBJECTTYPE_VISUALINDICATORDATA",
        "RWOBJECTTYPE_NAVMESH2DATA",
        "RWOBJECTTYPE_RENDEROPTIMESHDATA",
        "RWOBJECTTYPE_IRRADIANCEDATA",
        "RWOBJECTTYPE_ANTIFRUSTUMDATA",
        "RWOBJECTTYPE_SPLINESUBREF",
        "RWOBJECTTYPE_ROLLERDESCSUBREF",
        "RWOBJECTTYPE_RENDERMATERIALSUBREF",
        "RWOBJECTTYPE_COLLISIONMATERIALSUBREF",
        "RWOBJECTTYPE_LOCATIONDESCSUBREF",
        "RWOBJECTTYPE_INSTANCESUBREF",
        "RWOBJECTTYPE_WAYPOINTSUBREF",
        "RWOBJECTTYPE_TRIGGERINSTANCESUBREF",
        "RWOBJECTTYPE_EMITTERWAYPOINTSUBREF",
        "RWOBJECTTYPE_DMOSUBREF",
        "RWOBJECTTYPE_HOTPOINTSUBREF",
        "RWOBJECTTYPE_GRABSUBREF",
        "RWOBJECTTYPE_VISUALINDICATORSUBREF",
        "ARENADICTIONARY",
        "FORCENUMSIZEINT"
    };

    DWORD ArenaObjectTypeArray[]{
        /*RWOBJECTTYPE_NA*/  '\x00\x01\x00\x00',
        /*RWOBJECTTYPE_ARENA*/  '\x00\x01\x00\x01',
        /*RWOBJECTTYPE_RAW*/  '\x00\x01\x00\x02',
        /*RWOBJECTTYPE_SUBREFERENCE*/  '\x00\x01\x00\x03',
        /*RWOBJECTTYPE_SECTIONMANIFEST*/  '\x00\x01\x00\x04',
        /*RWOBJECTTYPE_SECTIONTYPES*/  '\x00\x01\x00\x05',
        /*RWOBJECTTYPE_SECTIONEXTERNALARENAS*/  '\x00\x01\x00\x06',
        /*RWOBJECTTYPE_SECTIONSUBREFERENCES*/  '\x00\x01\x00\x07',
        /*RWOBJECTTYPE_SECTIONATOMS*/  '\x00\x01\x00\x08',
        /*RWOBJECTTYPE_DEFARENAIMPORTS*/  '\x00\x01\x00\x09',
        /*RWOBJECTTYPE_DEFARENAEXPORTS*/  '\x00\x01\x00\x0A',
        /*RWOBJECTTYPE_DEFARENATYPES*/  '\x00\x01\x00\x0B',
        /*RWOBJECTTYPE_DEFARENADEFINEDARENAID*/  '\x00\x01\x00\x0C',
        /*RWOBJECTTYPE_ATTRIBUTEPACKET*/  '\x00\x01\x00\x0D',
        /*RWOBJECTTYPE_ATTRIBUTEPACKET_DELEGATE*/  '\x00\x01\x00\x0E',
        /*RWOBJECTTYPE_BITTABLE*/  '\x00\x01\x00\x0F',
        /*RWOBJECTTYPE_ARENALOCALATOMTABLE*/  '\x00\x01\x00\x10',
        /*RWOBJECTTYPE_BASERESOURCE_START*/  '\x00\x01\x00\x30',
        /*RWOBJECTTYPE_BASERESOURCE_1*/  '\x00\x01\x00\x31',
        /*RWOBJECTTYPE_BASERESOURCE_2*/  '\x00\x01\x00\x32',
        /*RWOBJECTTYPE_BASERESOURCE_3*/  '\x00\x01\x00\x33',
        /*RWOBJECTTYPE_BASERESOURCE_4*/  '\x00\x01\x00\x34',
        /*RWOBJECTTYPE_BASERESOURCE_5*/  '\x00\x01\x00\x35',
        /*RWOBJECTTYPE_BASERESOURCE_6*/  '\x00\x01\x00\x36',
        /*RWOBJECTTYPE_BASERESOURCE_7*/  '\x00\x01\x00\x37',
        /*RWOBJECTTYPE_BASERESOURCE_8*/  '\x00\x01\x00\x38',
        /*RWOBJECTTYPE_BASERESOURCE_9*/  '\x00\x01\x00\x39',
        /*RWOBJECTTYPE_BASERESOURCE_A*/  '\x00\x01\x00\x3A',
        /*RWOBJECTTYPE_BASERESOURCE_B*/  '\x00\x01\x00\x3B',
        /*RWOBJECTTYPE_BASERESOURCE_C*/  '\x00\x01\x00\x3C',
        /*RWOBJECTTYPE_BASERESOURCE_D*/  '\x00\x01\x00\x3D',
        /*RWOBJECTTYPE_BASERESOURCE_E*/  '\x00\x01\x00\x3E',
        /*RWOBJECTTYPE_BASERESOURCE_RESERVEDTO*/  '\x00\x01\x00\x3F',
        /*RWGOBJECTTYPE_NA1*/  '\x00\x02\x00\x00',
        /*RWGOBJECTTYPE_CAMERA*/  '\x00\x02\x00\x01',
        /*RWGOBJECTTYPE_PALETTE*/  '\x00\x02\x00\x02',
        /*RWGOBJECTTYPE_RASTER*/  '\x00\x02\x00\x03',
        /*RWGOBJECTTYPE_VDES*/  '\x00\x02\x00\x04',
        /*RWGOBJECTTYPE_VBUF*/  '\x00\x02\x00\x05',
        /*RWGOBJECTTYPE_IDES*/  '\x00\x02\x00\x06',
        /*RWGOBJECTTYPE_IBUF*/  '\x00\x02\x00\x07',
        /*RWGOBJECTTYPE_LIGHT*/  '\x00\x02\x00\x08',
        /*RWGOBJECTTYPE_MESH*/  '\x00\x02\x00\x09',
        /*RWGOBJECTTYPE_SHADER*/  '\x00\x02\x00\x0A',
        /*RWGOBJECTTYPE_COMPILEDSTATE*/  '\x00\x02\x00\x0B',
        /*RWGOBJECTTYPE_RENDEROBJECT*/  '\x00\x02\x00\x0C',
        /*RWGOBJECTTYPE_GSDATA*/  '\x00\x02\x00\x0D',
        /*RWGOBJECTTYPE_VERTEXSHADER*/  '\x00\x02\x00\x0E',
        /*RWGOBJECTTYPE_VERTEXDATA*/  '\x00\x02\x00\x0F',
        /*RWGOBJECTTYPE_INDEXDATA*/  '\x00\x02\x00\x10',
        /*RWGOBJECTTYPE_RASTERDATA*/  '\x00\x02\x00\x11',
        /*RWGOBJECTTYPE_BUILDSTATE*/  '\x00\x02\x00\x12',
        /*RWGOBJECTTYPE_PIXELSHADER*/  '\x00\x02\x00\x13',
        /*RWGOBJECTTYPE_PROFILEMESH*/  '\x00\x02\x00\x15',
        /*RWGOBJECTTYPE_DESIGNVIEWOBJECT*/  '\x00\x02\x00\x16',
        /*RWGOBJECTTYPE_PROFILERENDEROBJECT*/  '\x00\x02\x00\x17',
        /*RWGOBJECTTYPE_IMAGE*/  '\x00\x02\x00\x18',
        /*RWGOBJECTTYPE_RENDEROBJECTCONTAINER*/  '\x00\x02\x00\x19',
        /*RWGOBJECTTYPE_MESHCOMPILEDSTATELINK*/  '\x00\x02\x00\x1A',
        /*RWGOBJECTTYPE_SHADERCODE*/  '\x00\x02\x00\x1B',
        /*RWGOBJECTTYPE_FONT*/  '\x00\x02\x00\x20',
        /*RWGOBJECTTYPE_GLYPHTABLE*/  '\x00\x02\x00\x21',
        /*RWGOBJECTTYPE_KERNTABLE*/  '\x00\x02\x00\x22',
        /*RWGOBJECTTYPE_PAGETABLE*/  '\x00\x02\x00\x23',
        /*RWGOBJECTTYPE_RASTERTEXTURE*/  '\x00\x02\x00\x24',
        /*RWGOBJECTTYPE_FACENAME*/  '\x00\x02\x00\x25',
        /*RWGOBJECTTYPE_UPDATELOCATOR*/  '\x00\x02\x00\x7F',
        /*RWGOBJECTTYPE_MESHHELPER*/  '\x00\x02\x00\x81',
        /*RWGOBJECTTYPE_NA2*/  '\x00\x02\x00\xE0',
        /*RWGOBJECTTYPE_VIEWPORTPARAMETERS*/  '\x00\x02\x00\xE1',
        /*RWGOBJECTTYPE_SCISSORRECTPARAMETERS*/  '\x00\x02\x00\xE2',
        /*RWGOBJECTTYPE_CLEARCOLORPARAMETERS*/  '\x00\x02\x00\xE3',
        /*RWGOBJECTTYPE_CLEARDEPTHSTENCILPARAMETERS*/  '\x00\x02\x00\xE4',
        /*RWGOBJECTTYPE_DRAWINDEXEDPARAMETERS*/  '\x00\x02\x00\xE5',
        /*RWGOBJECTTYPE_DRAWPARAMETERS*/  '\x00\x02\x00\xE6',
        /*RWGOBJECTTYPE_PIXELBUFFER*/  '\x00\x02\x00\xE7',
        /*RWGOBJECTTYPE_TEXTURE*/  '\x00\x02\x00\xE8',
        /*RWGOBJECTTYPE_VERTEXDESCRIPTOR*/  '\x00\x02\x00\xE9',
        /*RWGOBJECTTYPE_VERTEXBUFFER*/  '\x00\x02\x00\xEA',
        /*RWGOBJECTTYPE_INDEXBUFFER*/  '\x00\x02\x00\xEB',
        /*RWGOBJECTTYPE_PROGRAMBUFFER*/  '\x00\x02\x00\xEC',
        /*RWGOBJECTTYPE_STATEBUFFER*/  '\x00\x02\x00\xED',
        /*RWGOBJECTTYPE_PROGRAMSTATEBUFFER*/  '\x00\x02\x00\xEE',
        /*RWGOBJECTTYPE_PROGRAMSTATES*/  '\x00\x02\x00\xEF',
        /*RWGOBJECTTYPE_SAMPLERSTATE*/  '\x00\x02\x00\xF1',
        /*RWGOBJECTTYPE_RENDERTARGETSTATE*/  '\x00\x02\x00\xF3',
        /*RWGOBJECTTYPE_BLENDSTATE*/  '\x00\x02\x00\xF4',
        /*RWGOBJECTTYPE_DEPTHSTENCILSTATE*/  '\x00\x02\x00\xF5',
        /*RWGOBJECTTYPE_RASTERIZERSTATE*/  '\x00\x02\x00\xF6',
        /*RWGOBJECTTYPE_VERTEXPROGRAMSTATE*/  '\x00\x02\x00\xF7',
        /*RWGOBJECTTYPE_PROGRAMSTATEHANDLE*/  '\x00\x02\x00\xF8',
        /*RWGOBJECTTYPE_DRAWINSTANCEDPARAMETERS*/  '\x00\x02\x00\xF9',
        /*RWGOBJECTTYPE_DRAWINDEXEDINSTANCEDPARAMETERS*/  '\x00\x02\x00\xFA',
        /*OBJECTTYPE_NA1*/  '\x00\x04\x00\x00',
        /*OBJECTTYPE_PART*/  '\x00\x04\x00\x01',
        /*OBJECTTYPE_PARTDEFINITION*/  '\x00\x04\x00\x02',
        /*OBJECTTYPE_JOINTEDPAIR*/  '\x00\x04\x00\x03',
        /*OBJECTTYPE_JOINTLIMIT*/  '\x00\x04\x00\x04',
        /*OBJECTTYPE_JOINTSKELETON*/  '\x00\x04\x00\x05',
        /*OBJECTTYPE_ASSEMBLY*/  '\x00\x04\x00\x06',
        /*OBJECTTYPE_ASSEMBLYDEFINITION*/  '\x00\x04\x00\x07',
        /*OBJECTTYPE_NA2*/  '\x00\x07\x00\x00',
        /*OBJECTTYPE_KEYFRAMEANIM*/  '\x00\x07\x00\x01',
        /*OBJECTTYPE_SKELETON*/  '\x00\x07\x00\x02',
        /*OBJECTTYPE_ANIMATIONSKIN*/  '\x00\x07\x00\x03',
        /*OBJECTTYPE_INTERPOLATOR*/  '\x00\x07\x00\x04',
        /*OBJECTTYPE_FEATHERINTERPOLATOR*/  '\x00\x07\x00\x05',
        /*OBJECTTYPE_ONEBONEIK*/  '\x00\x07\x00\x06',
        /*OBJECTTYPE_TWOBONEIK*/  '\x00\x07\x00\x07',
        /*OBJECTTYPE_BLENDER*/  '\x00\x07\x00\x08',
        /*OBJECTTYPE_WEIGHTEDBLENDER*/  '\x00\x07\x00\x09',
        /*OBJECTTYPE_REMAPPER*/  '\x00\x07\x00\x0A',
        /*OBJECTTYPE_SKELETONSINK*/  '\x00\x07\x00\x0B',
        /*OBJECTTYPE_SKINSINK*/  '\x00\x07\x00\x0C',
        /*OBJECTTYPE_LIGHTSINK*/  '\x00\x07\x00\x0D',
        /*OBJECTTYPE_CAMERASINK*/  '\x00\x07\x00\x0E',
        /*OBJECTTYPE_SKINMATRIXBUFFER*/  '\x00\x07\x00\x0F',
        /*OBJECTTYPE_TWEAKCONTROLLER*/  '\x00\x07\x00\x10',
        /*OBJECTTYPE_SHADERSINK*/  '\x00\x07\x00\x11',
        /*RWCOBJECTTYPE_NA*/  '\x00\x08\x00\x00',
        /*RWCOBJECTTYPE_VOLUME*/  '\x00\x08\x00\x01',
        /*RWCOBJECTTYPE_SIMPLEMAPPEDARRAY*/  '\x00\x08\x00\x02',
        /*RWCOBJECTTYPE_TRIANGLEKDTREEPROCEDURAL*/  '\x00\x08\x00\x03',
        /*RWCOBJECTTYPE_KDTREEMAPPEDARRAY*/  '\x00\x08\x00\x04',
        /*RWCOBJECTTYPE_BBOX*/  '\x00\x08\x00\x05',
        /*RWCOBJECTTYPE_CLUSTEREDMESH*/  '\x00\x08\x00\x06',
        /*RWOBJECTTYPE_RENDERMESHDATA*/  '\x00\xEB\x00\x00',
        /*RWOBJECTTYPE_RENDERMODELDATA*/  '\x00\xEB\x00\x01',
        /*RWOBJECTTYPE_SIMPLETRIMESHDATA*/  '\x00\xEB\x00\x03',
        /*RWOBJECTTYPE_SPLINEDATA*/  '\x00\xEB\x00\x04',
        /*RWOBJECTTYPE_RENDERMATERIALDATA*/  '\x00\xEB\x00\x05',
        /*RWOBJECTTYPE_COLLISIONMATERIALDATA*/  '\x00\xEB\x00\x06',
        /*RWOBJECTTYPE_ROLLERDESCDATA*/  '\x00\xEB\x00\x07',
        /*RWOBJECTTYPE_VERSIONDATA*/  '\x00\xEB\x00\x08',
        /*RWOBJECTTYPE_LOCATIONDESCDATA*/  '\x00\xEB\x00\x09',
        /*RWOBJECTTYPE_COLLISIONMODELDATA*/  '\x00\xEB\x00\x0A',
        /*RWOBJECTTYPE_TABLEOFCONTENTS*/  '\x00\xEB\x00\x0B',
        /*RWOBJECTTYPE_COLLISIONBEZIERDATA*/  '\x00\xEB\x00\x0C',
        /*RWOBJECTTYPE_INSTANCEDATA*/  '\x00\xEB\x00\x0D',
        /*RWOBJECTTYPE_RENDERBLENDSHAPEEDATA*/  '\x00\xEB\x00\x0E',
        /*RWOBJECTTYPE_WORLDPAINTERLAYERDATA*/  '\x00\xEB\x00\x0F',
        /*RWOBJECTTYPE_WORLDPAINTERQUADTREEDATA*/  '\x00\xEB\x00\x10',
        /*RWOBJECTTYPE_WORLDPAINTERDICTIONARYDATA*/  '\x00\xEB\x00\x11',
        /*RWOBJECTTYPE_NAVMESHDATA*/  '\x00\xEB\x00\x12',
        /*RWOBJECTTYPE_RAINDATA*/  '\x00\xEB\x00\x13',
        /*RWOBJECTTYPE_AIPATHDATA*/  '\x00\xEB\x00\x14',
        /*RWOBJECTTYPE_STATSDATA*/  '\x00\xEB\x00\x15',
        /*RWOBJECTTYPE_MASSIVEDATA*/  '\x00\xEB\x00\x16',
        /*RWOBJECTTYPE_DEPTHMAPDATA*/  '\x00\xEB\x00\x17',
        /*RWOBJECTTYPE_LIONDATA*/  '\x00\xEB\x00\x18',
        /*RWOBJECTTYPE_TRIGGERINSTANCEDATA*/  '\x00\xEB\x00\x19',
        /*RWOBJECTTYPE_WAYPOINTDATA*/  '\x00\xEB\x00\x1A',
        /*RWOBJECTTYPE_EMBEDDEDDATA*/  '\x00\xEB\x00\x1B',
        /*RWOBJECTTYPE_EMITTERWAYPOINTDATA*/  '\x00\xEB\x00\x1C',
        /*RWOBJECTTYPE_DMODATA*/  '\x00\xEB\x00\x1D',
        /*RWOBJECTTYPE_HOTPOINTDATA*/  '\x00\xEB\x00\x1E',
        /*RWOBJECTTYPE_GRABDATA*/  '\x00\xEB\x00\x1F',
        /*RWOBJECTTYPE_SPATIALMAP*/  '\x00\xEB\x00\x20',
        /*RWOBJECTTYPE_VISUALINDICATORDATA*/  '\x00\xEB\x00\x21',
        /*RWOBJECTTYPE_NAVMESH2DATA*/  '\x00\xEB\x00\x22',
        /*RWOBJECTTYPE_RENDEROPTIMESHDATA*/  '\x00\xEB\x00\x23',
        /*RWOBJECTTYPE_IRRADIANCEDATA*/  '\x00\xEB\x00\x24',
        /*RWOBJECTTYPE_ANTIFRUSTUMDATA*/  '\x00\xEB\x00\x25',
        /*RWOBJECTTYPE_SPLINESUBREF*/  '\x00\xEB\x00\x64',
        /*RWOBJECTTYPE_ROLLERDESCSUBREF*/  '\x00\xEB\x00\x65',
        /*RWOBJECTTYPE_RENDERMATERIALSUBREF*/  '\x00\xEB\x00\x66',
        /*RWOBJECTTYPE_COLLISIONMATERIALSUBREF*/  '\x00\xEB\x00\x67',
        /*RWOBJECTTYPE_LOCATIONDESCSUBREF*/  '\x00\xEB\x00\x68',
        /*RWOBJECTTYPE_INSTANCESUBREF*/  '\x00\xEB\x00\x69',
        /*RWOBJECTTYPE_WAYPOINTSUBREF*/  '\x00\xEB\x00\x6A',
        /*RWOBJECTTYPE_TRIGGERINSTANCESUBREF*/  '\x00\xEB\x00\x6B',
        /*RWOBJECTTYPE_EMITTERWAYPOINTSUBREF*/  '\x00\xEB\x00\x6C',
        /*RWOBJECTTYPE_DMOSUBREF*/  '\x00\xEB\x00\x6D',
        /*RWOBJECTTYPE_HOTPOINTSUBREF*/  '\x00\xEB\x00\x6E',
        /*RWOBJECTTYPE_GRABSUBREF*/  '\x00\xEB\x00\x6F',
        /*RWOBJECTTYPE_VISUALINDICATORSUBREF*/  '\x00\xEB\x00\x70',
        /*ARENADICTIONARY*/  '\x00\xEC\x00\x10',
        /*FORCENUMSIZEINT*/  '\x7F\xFF\xFF\xFF'
    };


    // I don't use this and it's not updated with all information from the other structures. (update if we ever need it.)
    enum ArenaObjectTypeEnum {
    RWOBJECTTYPE_NA = '\x00\x01\x00\x00',
    RWOBJECTTYPE_ARENA = '\x00\x01\x00\x01',
    RWOBJECTTYPE_RAW = '\x00\x01\x00\x02',
    RWOBJECTTYPE_SUBREFERENCE = '\x00\x01\x00\x03',
    RWOBJECTTYPE_SECTIONMANIFEST = '\x00\x01\x00\x04',
    RWOBJECTTYPE_SECTIONTYPES = '\x00\x01\x00\x05',
    RWOBJECTTYPE_SECTIONEXTERNALARENAS = '\x00\x01\x00\x06',
    RWOBJECTTYPE_SECTIONSUBREFERENCES = '\x00\x01\x00\x07',
    RWOBJECTTYPE_SECTIONATOMS = '\x00\x01\x00\x08',
    RWOBJECTTYPE_DEFARENAIMPORTS = '\x00\x01\x00\x09',
    RWOBJECTTYPE_DEFARENAEXPORTS = '\x00\x01\x00\x0A',
    RWOBJECTTYPE_DEFARENATYPES = '\x00\x01\x00\x0B',
    RWOBJECTTYPE_DEFARENADEFINEDARENAID = '\x00\x01\x00\x0C',
    RWOBJECTTYPE_ATTRIBUTEPACKET = '\x00\x01\x00\x0D',
    RWOBJECTTYPE_ATTRIBUTEPACKET_DELEGATE = '\x00\x01\x00\x0E',
    RWOBJECTTYPE_BITTABLE = '\x00\x01\x00\x0F',
    RWOBJECTTYPE_ARENALOCALATOMTABLE = '\x00\x01\x00\x10',
    RWOBJECTTYPE_BASERESOURCE_START = '\x00\x01\x00\x30',
    RWOBJECTTYPE_BASERESOURCE = '\x00\x01\x00\x31',
    RWOBJECTTYPE_BASERESOURCE_RESERVEDTO = '\x00\x01\x00\x3F',
    RWGOBJECTTYPE_NA1 = '\x00\x02\x00\x00',
    RWGOBJECTTYPE_CAMERA = '\x00\x02\x00\x01',
    RWGOBJECTTYPE_PALETTE = '\x00\x02\x00\x02',
    RWGOBJECTTYPE_RASTER = '\x00\x02\x00\x03',
    RWGOBJECTTYPE_VDES = '\x00\x02\x00\x04',
    RWGOBJECTTYPE_VBUF = '\x00\x02\x00\x05',
    RWGOBJECTTYPE_IDES = '\x00\x02\x00\x06',
    RWGOBJECTTYPE_IBUF = '\x00\x02\x00\x07',
    RWGOBJECTTYPE_LIGHT = '\x00\x02\x00\x08',
    RWGOBJECTTYPE_MESH = '\x00\x02\x00\x09',
    RWGOBJECTTYPE_SHADER = '\x00\x02\x00\x0A',
    RWGOBJECTTYPE_COMPILEDSTATE = '\x00\x02\x00\x0B',
    RWGOBJECTTYPE_RENDEROBJECT = '\x00\x02\x00\x0C',
    RWGOBJECTTYPE_GSDATA = '\x00\x02\x00\x0D',
    RWGOBJECTTYPE_VERTEXSHADER = '\x00\x02\x00\x0E',
    RWGOBJECTTYPE_VERTEXDATA = '\x00\x02\x00\x0F',
    RWGOBJECTTYPE_INDEXDATA = '\x00\x02\x00\x10',
    RWGOBJECTTYPE_RASTERDATA = '\x00\x02\x00\x11',
    RWGOBJECTTYPE_BUILDSTATE = '\x00\x02\x00\x12',
    RWGOBJECTTYPE_PIXELSHADER = '\x00\x02\x00\x13',
    RWGOBJECTTYPE_PROFILEMESH = '\x00\x02\x00\x15',
    RWGOBJECTTYPE_DESIGNVIEWOBJECT = '\x00\x02\x00\x16',
    RWGOBJECTTYPE_PROFILERENDEROBJECT = '\x00\x02\x00\x17',
    RWGOBJECTTYPE_IMAGE = '\x00\x02\x00\x18',
    RWGOBJECTTYPE_RENDEROBJECTCONTAINER = '\x00\x02\x00\x19',
    RWGOBJECTTYPE_MESHCOMPILEDSTATELINK = '\x00\x02\x00\x1A',
    RWGOBJECTTYPE_SHADERCODE = '\x00\x02\x00\x1B',
    RWGOBJECTTYPE_FONT = '\x00\x02\x00\x20',
    RWGOBJECTTYPE_GLYPHTABLE = '\x00\x02\x00\x21',
    RWGOBJECTTYPE_KERNTABLE = '\x00\x02\x00\x22',
    RWGOBJECTTYPE_PAGETABLE = '\x00\x02\x00\x23',
    RWGOBJECTTYPE_RASTERTEXTURE = '\x00\x02\x00\x24',
    RWGOBJECTTYPE_FACENAME = '\x00\x02\x00\x25',
    RWGOBJECTTYPE_UPDATELOCATOR = '\x00\x02\x00\x7F',
    RWGOBJECTTYPE_MESHHELPER = '\x00\x02\x00\x81',
    RWGOBJECTTYPE_NA2 = '\x00\x02\x00\xE0',
    RWGOBJECTTYPE_VIEWPORTPARAMETERS = '\x00\x02\x00\xE1',
    RWGOBJECTTYPE_SCISSORRECTPARAMETERS = '\x00\x02\x00\xE2',
    RWGOBJECTTYPE_CLEARCOLORPARAMETERS = '\x00\x02\x00\xE3',
    RWGOBJECTTYPE_CLEARDEPTHSTENCILPARAMETERS = '\x00\x02\x00\xE4',
    RWGOBJECTTYPE_DRAWINDEXEDPARAMETERS = '\x00\x02\x00\xE5',
    RWGOBJECTTYPE_DRAWPARAMETERS = '\x00\x02\x00\xE6',
    RWGOBJECTTYPE_PIXELBUFFER = '\x00\x02\x00\xE7',
    RWGOBJECTTYPE_TEXTURE = '\x00\x02\x00\xE8',
    RWGOBJECTTYPE_VERTEXDESCRIPTOR = '\x00\x02\x00\xE9',
    RWGOBJECTTYPE_VERTEXBUFFER = '\x00\x02\x00\xEA',
    RWGOBJECTTYPE_INDEXBUFFER = '\x00\x02\x00\xEB',
    RWGOBJECTTYPE_PROGRAMBUFFER = '\x00\x02\x00\xEC',
    RWGOBJECTTYPE_STATEBUFFER = '\x00\x02\x00\xED',
    RWGOBJECTTYPE_PROGRAMSTATEBUFFER = '\x00\x02\x00\xEE',
    RWGOBJECTTYPE_PROGRAMSTATES = '\x00\x02\x00\xEF',
    RWGOBJECTTYPE_SAMPLERSTATE = '\x00\x02\x00\xF1',
    RWGOBJECTTYPE_RENDERTARGETSTATE = '\x00\x02\x00\xF3',
    RWGOBJECTTYPE_BLENDSTATE = '\x00\x02\x00\xF4',
    RWGOBJECTTYPE_DEPTHSTENCILSTATE = '\x00\x02\x00\xF5',
    RWGOBJECTTYPE_RASTERIZERSTATE = '\x00\x02\x00\xF6',
    RWGOBJECTTYPE_VERTEXPROGRAMSTATE = '\x00\x02\x00\xF7',
    RWGOBJECTTYPE_PROGRAMSTATEHANDLE = '\x00\x02\x00\xF8',
    RWGOBJECTTYPE_DRAWINSTANCEDPARAMETERS = '\x00\x02\x00\xF9',
    RWGOBJECTTYPE_DRAWINDEXEDINSTANCEDPARAMETERS = '\x00\x02\x00\xFA',
    OBJECTTYPE_NA1 = '\x00\x04\x00\x00',
    OBJECTTYPE_PART = '\x00\x04\x00\x01',
    OBJECTTYPE_PARTDEFINITION = '\x00\x04\x00\x02',
    OBJECTTYPE_JOINTEDPAIR = '\x00\x04\x00\x03',
    OBJECTTYPE_JOINTLIMIT = '\x00\x04\x00\x04',
    OBJECTTYPE_JOINTSKELETON = '\x00\x04\x00\x05',
    OBJECTTYPE_ASSEMBLY = '\x00\x04\x00\x06',
    OBJECTTYPE_ASSEMBLYDEFINITION = '\x00\x04\x00\x07',
    OBJECTTYPE_NA2 = '\x00\x07\x00\x00',
    OBJECTTYPE_KEYFRAMEANIM = '\x00\x07\x00\x01',
    OBJECTTYPE_SKELETON = '\x00\x07\x00\x02',
    OBJECTTYPE_ANIMATIONSKIN = '\x00\x07\x00\x03',
    OBJECTTYPE_INTERPOLATOR = '\x00\x07\x00\x04',
    OBJECTTYPE_FEATHERINTERPOLATOR = '\x00\x07\x00\x05',
    OBJECTTYPE_ONEBONEIK = '\x00\x07\x00\x06',
    OBJECTTYPE_TWOBONEIK = '\x00\x07\x00\x07',
    OBJECTTYPE_BLENDER = '\x00\x07\x00\x08',
    OBJECTTYPE_WEIGHTEDBLENDER = '\x00\x07\x00\x09',
    OBJECTTYPE_REMAPPER = '\x00\x07\x00\x0A',
    OBJECTTYPE_SKELETONSINK = '\x00\x07\x00\x0B',
    OBJECTTYPE_SKINSINK = '\x00\x07\x00\x0C',
    OBJECTTYPE_LIGHTSINK = '\x00\x07\x00\x0D',
    OBJECTTYPE_CAMERASINK = '\x00\x07\x00\x0E',
    OBJECTTYPE_SKINMATRIXBUFFER = '\x00\x07\x00\x0F',
    OBJECTTYPE_TWEAKCONTROLLER = '\x00\x07\x00\x10',
    OBJECTTYPE_SHADERSINK = '\x00\x07\x00\x11',
    RWCOBJECTTYPE_NA = '\x00\x08\x00\x00',
    RWCOBJECTTYPE_VOLUME = '\x00\x08\x00\x01',
    RWCOBJECTTYPE_SIMPLEMAPPEDARRAY = '\x00\x08\x00\x02',
    RWCOBJECTTYPE_TRIANGLEKDTREEPROCEDURAL = '\x00\x08\x00\x03',
    RWCOBJECTTYPE_KDTREEMAPPEDARRAY = '\x00\x08\x00\x04',
    RWCOBJECTTYPE_BBOX = '\x00\x08\x00\x05',
    RWCOBJECTTYPE_CLUSTEREDMESH = '\x00\x08\x00\x06',
    RWOBJECTTYPE_RENDERMESHDATA = '\x00\xEB\x00\x00',
    RWOBJECTTYPE_RENDERMODELDATA = '\x00\xEB\x00\x01',
    RWOBJECTTYPE_SIMPLETRIMESHDATA = '\x00\xEB\x00\x03',
    RWOBJECTTYPE_SPLINEDATA = '\x00\xEB\x00\x04',
    RWOBJECTTYPE_RENDERMATERIALDATA = '\x00\xEB\x00\x05',
    RWOBJECTTYPE_COLLISIONMATERIALDATA = '\x00\xEB\x00\x06',
    RWOBJECTTYPE_ROLLERDESCDATA = '\x00\xEB\x00\x07',
    RWOBJECTTYPE_VERSIONDATA = '\x00\xEB\x00\x08',
    RWOBJECTTYPE_LOCATIONDESCDATA = '\x00\xEB\x00\x09',
    RWOBJECTTYPE_COLLISIONMODELDATA = '\x00\xEB\x00\x0A',
    RWOBJECTTYPE_TABLEOFCONTENTS = '\x00\xEB\x00\x0B',
    RWOBJECTTYPE_COLLISIONBEZIERDATA = '\x00\xEB\x00\x0C',
    RWOBJECTTYPE_INSTANCEDATA = '\x00\xEB\x00\x0D',
    RWOBJECTTYPE_RENDERBLENDSHAPEEDATA = '\x00\xEB\x00\x0E',
    RWOBJECTTYPE_WORLDPAINTERLAYERDATA = '\x00\xEB\x00\x0F',
    RWOBJECTTYPE_WORLDPAINTERQUADTREEDATA = '\x00\xEB\x00\x10',
    RWOBJECTTYPE_WORLDPAINTERDICTIONARYDATA = '\x00\xEB\x00\x11',
    RWOBJECTTYPE_NAVMESHDATA = '\x00\xEB\x00\x12',
    RWOBJECTTYPE_RAINDATA = '\x00\xEB\x00\x13',
    RWOBJECTTYPE_AIPATHDATA = '\x00\xEB\x00\x14',
    RWOBJECTTYPE_STATSDATA = '\x00\xEB\x00\x15',
    RWOBJECTTYPE_MASSIVEDATA = '\x00\xEB\x00\x16',
    RWOBJECTTYPE_DEPTHMAPDATA = '\x00\xEB\x00\x17',
    RWOBJECTTYPE_LIONDATA = '\x00\xEB\x00\x18',
    RWOBJECTTYPE_TRIGGERINSTANCEDATA = '\x00\xEB\x00\x19',
    RWOBJECTTYPE_WAYPOINTDATA = '\x00\xEB\x00\x1A',
    RWOBJECTTYPE_EMBEDDEDDATA = '\x00\xEB\x00\x1B',
    RWOBJECTTYPE_EMITTERWAYPOINTDATA = '\x00\xEB\x00\x1C',
    RWOBJECTTYPE_DMODATA = '\x00\xEB\x00\x1D',
    RWOBJECTTYPE_HOTPOINTDATA = '\x00\xEB\x00\x1E',
    RWOBJECTTYPE_GRABDATA = '\x00\xEB\x00\x1F',
    RWOBJECTTYPE_SPATIALMAP = '\x00\xEB\x00\x20',
    RWOBJECTTYPE_VISUALINDICATORDATA = '\x00\xEB\x00\x21',
    RWOBJECTTYPE_NAVMESH2DATA = '\x00\xEB\x00\x22',
    RWOBJECTTYPE_RENDEROPTIMESHDATA = '\x00\xEB\x00\x23',
    RWOBJECTTYPE_IRRADIANCEDATA = '\x00\xEB\x00\x24',
    RWOBJECTTYPE_ANTIFRUSTUMDATA = '\x00\xEB\x00\x25',
    RWOBJECTTYPE_SPLINESUBREF = '\x00\xEB\x00\x64',
    RWOBJECTTYPE_ROLLERDESCSUBREF = '\x00\xEB\x00\x65',
    RWOBJECTTYPE_RENDERMATERIALSUBREF = '\x00\xEB\x00\x66',
    RWOBJECTTYPE_COLLISIONMATERIALSUBREF = '\x00\xEB\x00\x67',
    RWOBJECTTYPE_LOCATIONDESCSUBREF = '\x00\xEB\x00\x68',
    RWOBJECTTYPE_INSTANCESUBREF = '\x00\xEB\x00\x69',
    RWOBJECTTYPE_WAYPOINTSUBREF = '\x00\xEB\x00\x6A',
    RWOBJECTTYPE_TRIGGERINSTANCESUBREF = '\x00\xEB\x00\x6B',
    RWOBJECTTYPE_EMITTERWAYPOINTSUBREF = '\x00\xEB\x00\x6C',
    RWOBJECTTYPE_DMOSUBREF = '\x00\xEB\x00\x6D',
    RWOBJECTTYPE_HOTPOINTSUBREF = '\x00\xEB\x00\x6E',
    RWOBJECTTYPE_GRABSUBREF = '\x00\xEB\x00\x6F',
    RWOBJECTTYPE_VISUALINDICATORSUBREF = '\x00\xEB\x00\x70',
    ARENADICTIONARY = '\x00\xEC\x00\x10',
    FORCENUMSIZEINT = '\x7F\xFF\xFF\xFF'
    };


    // Function to unpack files from an Arena file package.
    auto parse_arena_filepackage(std::wstring ArchivePath, bool Unpack, int64_t selected_file_index) {
        // Declare local variables.
        FILE* archive = nullptr;
        FILE* file = nullptr;
        std::vector<Archive_Parse_Struct> Archive_Parse_Struct_vector = {};
        std::wstring out_directory = ParseFilePath(ArchivePath).first + GetFilenameWithoutExtension(ArchivePath.c_str());

        struct RESULT { std::vector<Archive_Parse_Struct>  parsed_info; bool success; };
        
        _wfopen_s(&archive, ArchivePath.c_str(), L"rb");
        if (archive == NULL) {
            perror("Error opening archive");
            return RESULT{ Archive_Parse_Struct_vector , false };
        }

        // Save start position.
        uint64_t start_of_archive = _ftelli64(archive);

        // Save archive size and go back to start position.
        fseek(archive, 0, SEEK_END);
        uint64_t archive_size = _ftelli64(archive);
        fseek(archive, start_of_archive, SEEK_SET);

        _fseeki64(archive, 32, SEEK_SET); // Seek to file_count.

        // Read filecount and convert to little endian.
        uint32_t file_count = 0;
        fread(&file_count, sizeof(uint32_t), 1, archive);
        file_count = BigToLittleUINT(file_count);

        _fseeki64(archive, 48, SEEK_SET); // Seek to file_table_offset

        // Read file table offset and convert to little endian.
        uint32_t file_table_offset = 0;
        fread(&file_table_offset, sizeof(uint32_t), 1, archive);
        file_table_offset = BigToLittleUINT(file_table_offset);

        if (file_table_offset > archive_size)
        {
            fclose(archive);
            return RESULT{ Archive_Parse_Struct_vector , false };
        }

        // Loop through all Arena objects.
        uint32_t object_index = 0;
        for (size_t i = 0; i < file_count; i++)
        {
            Archive_Parse_Struct Parsed_Archive_Struct = {};

            std::wstring out_path = out_directory + L"\\";
            bool name_found = false;

            _fseeki64(archive, file_table_offset + (i*24), SEEK_SET); // Seek to file toc.

            // Read TOC data and convert to little endian.
            uint32_t offset = 0;
            fread(&offset, sizeof(uint32_t), 1, archive);
            offset = BigToLittleUINT(offset);

            uint64_t size = 0;
            fread(&size, sizeof(uint64_t), 1, archive);
            size = BigToLittleUINT(size);

            uint64_t unknown1 = 0;
            fread(&unknown1, sizeof(uint32_t), 1, archive);
            unknown1 = BigToLittleUINT(unknown1);

            uint64_t unknown2 = 0;
            fread(&unknown2, sizeof(uint32_t), 1, archive);
            unknown2 = BigToLittleUINT(unknown2);

            uint32_t file_type = 0;
            fread(&file_type, sizeof(uint32_t), 1, archive);
            file_type = BigToLittleUINT(file_type);

            if (offset > archive_size || size > archive_size)
            {
                fclose(archive);
                return RESULT{ Archive_Parse_Struct_vector , false };
            }

            size_t object_name_index = 0;
            for (size_t j = 0; j < sizeof(ArenaObjectTypeArray) / sizeof(ArenaObjectTypeArray[0]); j++)
            {
                if (BigToLittleUINT(ArenaObjectTypeArray[j]) == file_type)
                {
                    object_name_index = j;
                    name_found = true;
                    break;
                }
            }

            object_index++; // Add 1 to object index.
            
            // Check if we found a name that matches the object ID.
            if (name_found == true)
            {
                out_path += L"Object_" + std::to_wstring(object_index) + L"_" + to_wstring(ArenaObjectTypeNames[object_name_index]);
                Parsed_Archive_Struct.filename = ArenaObjectTypeNames[object_name_index];
            }
            else
            {
                out_path += L"Object_" + std::to_wstring(object_index) + L"_" + L"UNKNOWN_TYPE";
                Parsed_Archive_Struct.filename = "UNKNOWN_TYPE";
            }

            // If offset is 0, handle differently and assume the offset is the start of the first Arena Object.
            if (offset == 0)
            {
                _fseeki64(archive, 68, SEEK_SET);
                uint32_t local_size = 0;
                fread(&local_size, sizeof(uint32_t), 1, archive);
                local_size = BigToLittleUINT(local_size);
                offset = offset + local_size;
            }

            // Build parse struct.
            Parsed_Archive_Struct.file_offset = offset;
            Parsed_Archive_Struct.file_size = size;
            Parsed_Archive_Struct.toc_offset = file_table_offset + (i * 24);

            _fseeki64(archive, offset, SEEK_SET); // Seek to file offset.

            bool full_archive_unpack = Unpack && selected_file_index == -1; // Do a full archive unpack.
            bool single_archive_unpack = Unpack && selected_file_index == i; // Do a single file archive unpack.

            if (full_archive_unpack || single_archive_unpack)
            {
                // Attempt to create the directory
                if (CreateDirectoryRecursively(out_directory.c_str())) {
                    wprintf(L"Directory created: %s\n", out_directory.c_str());
                }
                else {
                    wprintf(L"Failed to create directory or directory already exists: %s\n", out_directory.c_str());
                }

                char* out_buffer = (char*)malloc(size);
                if (out_buffer == nullptr)
                {
                    fclose(archive);
                    MessageBox(0, L"Error creating Arena out_buffer!  \nUnpacker is unable to continue!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    return RESULT { Archive_Parse_Struct_vector , false };
                }

                fread(out_buffer, size, 1, archive);

                // Write to file.
                if (!IoTools::write_file(file, out_path, out_buffer, size))
                {
                    fclose(archive);
                    free(out_buffer);
                    MessageBox(0, L"Error writing data! \nMake sure you don't have a handle to a file from some other tool! \nUnpacker is unable to continue!", L"Unpacker Prompt", MB_OK | MB_ICONINFORMATION);
                    return RESULT{ Archive_Parse_Struct_vector , false };
                }

                free(out_buffer);
            }

            Archive_Parse_Struct_vector.push_back(Parsed_Archive_Struct);
        }

        fclose(archive);
        return RESULT { Archive_Parse_Struct_vector , true };
    }
}