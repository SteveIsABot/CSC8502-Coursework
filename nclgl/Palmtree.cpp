#include "../nclgl/Palmtree.h"

Palmtree::Palmtree(Mesh* cone, Mesh* sphere, GLuint barkTex, GLuint leafTex) {

    Vector4 trunkColour = Vector4(100.0f / 255.0f, 98.0f / 255.0f, 49.0f / 255.0f, 1.0);
    Vector4 leafColour = Vector4(0, 1, 0, 1);

    treeBase = new SceneNode(cone, trunkColour);
    treeBase->SetModelScale(Vector3(100, 100, 700));
    treeBase->SetTransform(treeBase->GetTransform() * Matrix4::Rotation(-90, Vector3(1, 0, 0)));
    treeBase->SetTexture(barkTex);
    AddChild(treeBase);

    leaves = new SceneNode(sphere, leafColour);
    leaves->SetModelScale(Vector3(10, 10, 10));
    leaves->SetTransform(Matrix4::Translation(Vector3(0, 700, 0)));
    leaves->SetTexture(leafTex);
    AddChild(leaves);

    SceneNode* leafA = new SceneNode(cone, leafColour);
    leafA->SetModelScale(Vector3(150, 10, 250));
    leafA->SetTransform(Matrix4::Translation(Vector3(144, -145, 0)));
    leafA->SetTransform(leafA->GetTransform() * Matrix4::Rotation(-90, Vector3(0, 1, 0)));
    leafA->SetTransform(leafA->GetTransform() * Matrix4::Rotation(-45, Vector3(1, 0, 0)));
    leafA->SetTexture(leafTex);
    leaves->AddChild(leafA);

    SceneNode* leafB = new SceneNode(cone, leafColour);
    leafB->SetModelScale(Vector3(150, 10, 250));
    leafB->SetTransform(Matrix4::Translation(Vector3(-144, -145, 0)));
    leafB->SetTransform(leafB->GetTransform() * Matrix4::Rotation(90, Vector3(0, 1, 0)));
    leafB->SetTransform(leafB->GetTransform() * Matrix4::Rotation(-45, Vector3(1, 0, 0)));
    leafB->SetTexture(leafTex);
    leaves->AddChild(leafB);

    SceneNode* leafC = new SceneNode(cone, leafColour);
    leafC->SetModelScale(Vector3(150, 10, 250));
    leafC->SetTransform(Matrix4::Translation(Vector3(0, -145, -144)));
    leafC->SetTransform(leafC->GetTransform() * Matrix4::Rotation(-45, Vector3(1, 0, 0)));
    leafC->SetTexture(leafTex);
    leaves->AddChild(leafC);

    SceneNode* leafD = new SceneNode(cone, leafColour);
    leafD->SetModelScale(Vector3(150, 10, 250));
    leafD->SetTransform(Matrix4::Translation(Vector3(0, -145, 144)));
    leafD->SetTransform(leafD->GetTransform() * Matrix4::Rotation(180, Vector3(0, 1, 0)));
    leafD->SetTransform(leafD->GetTransform() * Matrix4::Rotation(-45, Vector3(1, 0, 0)));
    leafD->SetTexture(leafTex);
    leaves->AddChild(leafD);
}