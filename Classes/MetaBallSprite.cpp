#include "MetaBallSprite.h"
#include "Resources\shader\metaball_shader.glsl"
USING_NS_CC;

MetaBallSprite* MetaBallSprite::create(const std::string& path)
{
    auto ins = new MetaBallSprite(path);
    ins->initWithFile(path);
    ins->autorelease();
    ins->init();
    return ins;
}

MetaBallSprite::MetaBallSprite(const std::string& path): Sprite()
{
  
}

MetaBallSprite::~MetaBallSprite()
{
}

void MetaBallSprite::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    //_glProgramState->apply(transform);

    _glProgramState->setUniformVec2("_center", getPosition());

    Sprite::draw(renderer, transform, flags);
    renderer->render();
}

bool MetaBallSprite::init()
{
    GLProgram* glprogram = GLProgram::createWithByteArrays(metaball_vs, metaball_fs);
    auto state = GLProgramState::getOrCreateWithGLProgram(glprogram);
    this->setGLProgramState(state);
    auto mouse = EventListenerMouse::create();
    state->setUniformVec2("resolution", Director::getInstance()->getWinSize());
    auto size = Director::getInstance()->getVisibleSize();
    auto radius = std::min(getTexture()->getContentSize().width / size.width, getTexture()->getContentSize().height/ size.height);
   
    state->setUniformFloat("radius", radius);
    
    mouse->onMouseMove = [this, size](EventMouse* e) {
            _glProgramState->setUniformVec2("mouse", { e->getCursorX() / size.width , e->getCursorY() / size.height });
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse, this);

   /* _glProgramState->setVertexAttribPointer("a_position", 2, GL_FLOAT, GL_FALSE, 0, _polyInfo.triangles.verts);
    _glProgramState->setVertexAttribPointer("a_texCoord", 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), _polyInfo.triangles.verts);*/
    return true;
}
