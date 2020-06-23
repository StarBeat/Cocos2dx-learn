#include "Effect.h"
using namespace cocos2d;
using std::string;

bool Effect::initGLProgramState(const std::string& fs, const std::string& vs)
{
    FileUtils* fileUtil = FileUtils::getInstance();
    string vsStr, fsStr;
    if (!vs.empty())
    {
        string vsPath = fileUtil->fullPathForFilename(vs);
        vsStr = fileUtil->getStringFromFile(vsPath);
    }
    else
    {
        vsStr = ccPositionTextureColor_noMVP_vert;
    }
    string fsPath = fileUtil->fullPathForFilename(fs);
    fsStr = fileUtil->getStringFromFile(fsPath);
    GLProgram* glprogram = GLProgram::createWithByteArrays(vsStr.c_str(), fsPath.c_str());

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
}
