#include "Effect.h"
using namespace cocos2d;
using std::string;

bool Effect::initGLProgramState(const std::string& vs, const std::string& fs)
{
    string vsStr = vs, fsStr = fs;
    if (vsStr.empty())
    {
        vsStr = ccPositionTextureColor_noMVP_vert;
    }
    if (fsStr.empty())
    {
        fsStr = ccPositionTextureColor_noMVP_frag;
    }
    GLProgram* glprogram = GLProgram::createWithByteArrays(vsStr.c_str(), fsStr.c_str());

    _glprogramstate = GLProgramState::getOrCreateWithGLProgram(glprogram);
    _glprogramstate->retain();

    return _glprogramstate != nullptr;;
}

Effect::Effect():_glprogramstate(nullptr)
{

}

Effect::~Effect()
{
    CC_SAFE_RELEASE(_glprogramstate);
    this->release();
}
