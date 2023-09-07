#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <vector>
#include "../third_party/CLI11.hpp"
#include "fbxsdk.h"

#ifdef IOS_REF
#undef IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif
/**
 * Save FbxScene
 */
bool CommonSaveScene(fbxsdk::FbxManager* pManager, fbxsdk::FbxDocument* pScene, const char* pFilename, int pFileFormat = -1, bool pEmbedMedia = false) {
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    fbxsdk::FbxExporter* lExporter = fbxsdk::FbxExporter::Create(pManager, "");

    if (pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount()) {
        // Write in fall back format in less no ASCII format found
        pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        // Try to export in ASCII if possible
        int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

        for (lFormatIndex = 0; lFormatIndex < lFormatCount; lFormatIndex++) {
            if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex)) {
                FbxString lDesc = pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                const char* lASCII = "ascii";
                if (lDesc.Find(lASCII) >= 0) {
                    pFileFormat = lFormatIndex;
                    break;
                }
            }
        }
    }

    // Set the export states. By default, the export states are always set to
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below
    // shows how to change these states.
    IOS_REF.SetBoolProp(EXP_FBX_MATERIAL, true);
    IOS_REF.SetBoolProp(EXP_FBX_TEXTURE, true);
    IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED, pEmbedMedia);
    IOS_REF.SetBoolProp(EXP_FBX_SHAPE, true);
    IOS_REF.SetBoolProp(EXP_FBX_GOBO, true);
    IOS_REF.SetBoolProp(EXP_FBX_ANIMATION, true);
    IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if (lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false) {
        FBXSDK_printf("Call to FbxExporter::Initialize() failed.\n");
        FBXSDK_printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
        return false;
    }

    fbxsdk::FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    // FBXSDK_printf("FBX file format version %d.%d.%d\n\n", lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(pScene);

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}

/**
 * Return a string-based representation based on the attribute type.
 */
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
    switch (type) {
        case FbxNodeAttribute::eUnknown:
            return "unidentified";
        case FbxNodeAttribute::eNull:
            return "null";
        case FbxNodeAttribute::eMarker:
            return "marker";
        case FbxNodeAttribute::eSkeleton:
            return "skeleton";
        case FbxNodeAttribute::eMesh:
            return "mesh";
        case FbxNodeAttribute::eNurbs:
            return "nurbs";
        case FbxNodeAttribute::ePatch:
            return "patch";
        case FbxNodeAttribute::eCamera:
            return "camera";
        case FbxNodeAttribute::eCameraStereo:
            return "stereo";
        case FbxNodeAttribute::eCameraSwitcher:
            return "camera switcher";
        case FbxNodeAttribute::eLight:
            return "light";
        case FbxNodeAttribute::eOpticalReference:
            return "optical reference";
        case FbxNodeAttribute::eOpticalMarker:
            return "marker";
        case FbxNodeAttribute::eNurbsCurve:
            return "nurbs curve";
        case FbxNodeAttribute::eTrimNurbsSurface:
            return "trim nurbs surface";
        case FbxNodeAttribute::eBoundary:
            return "boundary";
        case FbxNodeAttribute::eNurbsSurface:
            return "nurbs surface";
        case FbxNodeAttribute::eShape:
            return "shape";
        case FbxNodeAttribute::eLODGroup:
            return "lodgroup";
        case FbxNodeAttribute::eSubDiv:
            return "subdiv";
        default:
            return "unknown";
    }
}

/**
 * Print Node Transfroms，Including Global,Local,GetGeometric and Lcl
 */
void PrintNodeTransform(fbxsdk::FbxNode* node) {
    // Global
    // Global-Translation
    std::cout << "Global-Translation: (" << node->EvaluateGlobalTransform().GetT()[0]
              << ", " << node->EvaluateGlobalTransform().GetT()[1]
              << ", " << node->EvaluateGlobalTransform().GetT()[2]
              << ", " << node->EvaluateGlobalTransform().GetT()[3]
              << ")" << std::endl;
    // Global-Rotation
    std::cout << "Global-Rotation: (" << node->EvaluateGlobalTransform().GetR()[0]
              << ", " << node->EvaluateGlobalTransform().GetR()[1]
              << ", " << node->EvaluateGlobalTransform().GetR()[2]
              << ", " << node->EvaluateGlobalTransform().GetR()[3]
              << ")" << std::endl;
    // Global-Scaling
    std::cout << "Global-Scaling: (" << node->EvaluateGlobalTransform().GetS()[0]
              << ", " << node->EvaluateGlobalTransform().GetS()[1]
              << ", " << node->EvaluateGlobalTransform().GetS()[2]
              << ", " << node->EvaluateGlobalTransform().GetS()[3]
              << ")" << std::endl;
    // Global-Quaternion
    std::cout << "Global-Quaternion: (" << node->EvaluateGlobalTransform().GetQ()[0]
              << ", " << node->EvaluateGlobalTransform().GetQ()[1]
              << ", " << node->EvaluateGlobalTransform().GetQ()[2]
              << ", " << node->EvaluateGlobalTransform().GetQ()[3]
              << ")" << std::endl;

    // Local
    // Local-Translation
    std::cout << "Local-Translation: (" << node->EvaluateLocalTranslation()[0]
              << ", " << node->EvaluateLocalTranslation()[1]
              << ", " << node->EvaluateLocalTranslation()[2]
              << ", " << node->EvaluateLocalTranslation()[3]
              << ")" << std::endl;
    // Local-Rotation
    std::cout << "Local-Rotation: (" << node->EvaluateLocalRotation()[0]
              << ", " << node->EvaluateLocalRotation()[1]
              << ", " << node->EvaluateLocalRotation()[2]
              << ", " << node->EvaluateLocalRotation()[3]
              << ")" << std::endl;
    // Local-Scaling
    std::cout << "Local-Scaling: (" << node->EvaluateLocalScaling()[0]
              << ", " << node->EvaluateLocalScaling()[1]
              << ", " << node->EvaluateLocalScaling()[2]
              << ", " << node->EvaluateLocalScaling()[3]
              << ")" << std::endl;

    // Geometric
    // Geometric-Translation
    std::cout << "Geometric-Translation: (" << node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot)[0]
              << ", " << node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot)[1]
              << ", " << node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot)[2]
              << ", " << node->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot)[3]
              << ")" << std::endl;
    // Geometric-Rotation
    std::cout << "Geometric-Rotation: (" << node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot)[0]
              << ", " << node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot)[1]
              << ", " << node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot)[2]
              << ", " << node->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot)[3]
              << ")" << std::endl;
    // Geometric-Scaling
    std::cout << "Geometric-Scaling: (" << node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot)[0]
              << ", " << node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot)[1]
              << ", " << node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot)[2]
              << ", " << node->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot)[3]
              << ")" << std::endl;

    // LCL (same as Local-Translation)
    // LCL-Translation
    std::cout << "LCL-Translation: (" << node->LclTranslation.Get()[0]
              << ", " << node->LclTranslation.Get()[1]
              << ", " << node->LclTranslation.Get()[2]
              << ")" << std::endl;
    // LCL-Rotation
    std::cout << "LCL-Rotation: (" << node->LclRotation.Get()[0]
              << ", " << node->LclRotation.Get()[1]
              << ", " << node->LclRotation.Get()[2]
              << ")" << std::endl;
    // LCL-Scaling
    std::cout << "LCL-Scaling: (" << node->LclScaling.Get()[0]
              << ", " << node->LclScaling.Get()[1]
              << ", " << node->LclScaling.Get()[2]
              << ")" << std::endl;
}

/**
 * Create new scene from node
 */
fbxsdk::FbxScene* NewScene(fbxsdk::FbxManager* lSdkManager, fbxsdk::FbxNode* node, fbxsdk::FbxSystemUnit unit) {
    fbxsdk::FbxScene* scene = fbxsdk::FbxScene::Create(lSdkManager, node->GetName());
    unit.ConvertScene(scene);  // Unified unit

    // change transform
    // PrintNodeTransform(node);
    fbxsdk::FbxAMatrix matrix = node->EvaluateGlobalTransform();
    node->LclTranslation.Set(fbxsdk::FbxDouble3(matrix.GetT()[0], matrix.GetT()[1], matrix.GetT()[2]));
    node->LclRotation.Set(fbxsdk::FbxDouble3(matrix.GetR()[0], matrix.GetR()[1], matrix.GetR()[2]));
    node->LclScaling.Set(fbxsdk::FbxDouble3(matrix.GetS()[0], matrix.GetS()[1], matrix.GetS()[2]));

    scene->ConnectSrcObject(node->GetMesh());  // link mesh

    for (int i = 0; i < node->GetMaterialCount(); i++) {
        fbxsdk::FbxSurfaceMaterial* material = node->GetSrcObject<fbxsdk::FbxSurfaceMaterial>(i);
        // std::cout << "material name: " << material->GetName() << std::endl;
        scene->ConnectSrcObject(material);
        for (int k = 0; k < fbxsdk::FbxLayerElement::sTypeTextureCount; k++) {
            fbxsdk::FbxProperty property = material->FindProperty(fbxsdk::FbxLayerElement::sTextureChannelNames[k]);
            // if (property.GetName() != "") {
            //     // std::cout << "property: " << property.GetName() << std::endl;
            // }
            for (int l = 0; l < property.GetSrcObjectCount(fbxsdk::FbxCriteria::ObjectTypeStrict(fbxsdk::FbxFileTexture::ClassId)); l++) {
                fbxsdk::FbxObject* texture = property.GetSrcObject(fbxsdk::FbxCriteria::ObjectTypeStrict(fbxsdk::FbxFileTexture::ClassId), l);
                // std::cout << "\ttexture: " << static_cast<fbxsdk::FbxFileTexture*>(texture)->GetFileName() << std::endl;
                scene->ConnectSrcObject(texture);
            }
        }
    }
    scene->ConnectSrcObject(node);
    scene->GetRootNode()->AddChild(node);

    return scene;
}

/**
 * Recursively search nodes who has mesh
 */
void FindMeshNode(fbxsdk::FbxNode* node, std::vector<fbxsdk::FbxNode*>* nodes) {
    for (int i = 0; i < node->GetChildCount(); i++) {
        FindMeshNode(node->GetChild(i), nodes);
    }
    if (node->GetMesh()) {
        nodes->push_back(node);
    }
}

/**
 * Combine directory and filename into fullpath
 */
char* CombinePath(const char* dir, const char* fileName) {
    char* fullPath = new char[1024];
    strcpy(fullPath, dir);
    if (dir[strlen(dir) - 1] != '/') {
        strcat(fullPath, "/");
    }
    strcat(fullPath, fileName);
    // std::cout << "fullPath: " << fullPath << std::endl;

    return fullPath;
}

/**
 * Create floder if folder not exist
 */
void CheckFolder(const char* dir) {
    DIR* folder = opendir(dir);
    if (folder != NULL) {
        std::cout << "folder exists: " << dir << std::endl;
        return;
    } else {
        if (mkdir(dir, S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO) == 0) {
            std::cout << "create floder: " << dir << std::endl;
        }
    }
}

/**
 * Split fbx according to nodes who has mesh
 */
void FbxSplit(const char* path, const char* outputFolder) {
    // Initialize the SDK manager. This object handles all our memory management.
    fbxsdk::FbxManager* lSdkManager = fbxsdk::FbxManager::Create();

    // Create the IO settings object.
    fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(lSdkManager, IOSROOT);
    lSdkManager->SetIOSettings(ios);

    // Create an importer using the SDK manager.
    fbxsdk::FbxImporter* lImporter = fbxsdk::FbxImporter::Create(lSdkManager, "");

    // Use the first argument as the filename for the importer.
    if (!lImporter->Initialize(path, -1, lSdkManager->GetIOSettings())) {
        printf("Call to FbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
        std::getchar();
        exit(-1);
    }

    // Create a new scene so that it can be populated by the imported file.
    fbxsdk::FbxScene* lScene = fbxsdk::FbxScene::Create(lSdkManager, "myScene");

    // Import the contents of the file into the scene.
    lImporter->Import(lScene);

    // The file is imported; so get rid of the importer.
    lImporter->Destroy();

    // PrintUnit of lScene
    fbxsdk::FbxSystemUnit unit = lScene->GetGlobalSettings().GetSystemUnit();
    std::cout << "lScene's unit: " << unit.GetScaleFactorAsString() << std::endl;

    // Print the nodes of the scene and their attributes recursively.
    // Note that we are not printing the root node because it should
    // not contain any attributes.
    fbxsdk::FbxNode* lRootNode = lScene->GetRootNode();

    std::cout << "RootNode: " << lRootNode->GetName() << " - ChildCount: " << lRootNode->GetChildCount() << std::endl;

    std::vector<fbxsdk::FbxNode*> nodes;
    std::vector<fbxsdk::FbxScene*> scenes;
    FindMeshNode(lRootNode, &nodes);

    for (fbxsdk::FbxNode* node : nodes) {
        // std::cout << "---------- MeshNode: " << node->GetName() << std::endl;
        fbxsdk::FbxScene* scene = NewScene(lSdkManager, node, unit);
        scenes.push_back(scene);
    }

    std::map<std::string, int> m;
    std::vector<const char*> names;

    for (int i = 0; i < scenes.size(); i++) {
        // std::cout << "Scene: " << scenes[i]->GetName() << std::endl;
        if (m.find(scenes[i]->GetName()) == m.end()) {  // element not exist
            names.push_back(scenes[i]->GetName());
            m.insert(std::pair<std::string, int>(scenes[i]->GetName(), 1));
        } else {
            char* name = new char[1024];
            sprintf(name, "%s_%d", scenes[i]->GetName(), m[scenes[i]->GetName()]);
            names.push_back(name);
            m[scenes[i]->GetName()]++;
        }
    }

    for (auto name : names) {
        std::cout << "Scene Name:" << name << std::endl;
    }

    lScene->DisconnectAllSrcObject();  // Remove lScene links
    std::cout << "Scene Count: " << scenes.size() << std::endl;

    CheckFolder(outputFolder);

    for (int i = 0; i < scenes.size(); i++) {
        CommonSaveScene(lSdkManager, scenes[i], CombinePath(outputFolder, names[i]), 0);
        // Destory scenes and links
        scenes[i]->DisconnectAllSrcObject();
        scenes[i]->Destroy();
    }
}

int main(int argc, char** argv) {
    CLI::App app{"FBX-SPLIT v0.1"};
    // Descript parameters
    std::string input;
    std::string output = "./out/";
    // Add flag
    app.add_option("-i,--input", input, "输入的FBX模型文件")->check(CLI::ExistingFile);
    app.add_option("-o,--output", output, "输出模型文件的目录");

    CLI11_PARSE(app, argc, argv);

    std::cout << "input: " << input << std::endl;
    std::cout << "output: " << output << std::endl;

    FbxSplit(input.c_str(), output.c_str());

    return 0;
}