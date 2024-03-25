#pragma once
#include "../nclgl/SceneNode.h"

class Palmtree : public SceneNode {

public:
    Palmtree(Mesh* cone, Mesh* sphere, GLuint barkTex, GLuint leafTex);
    ~Palmtree(void) {};

protected:
    SceneNode* treeBase;
    SceneNode* leaves;
};