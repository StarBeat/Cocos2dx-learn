#include "PuddingEffect.h"
#include "Resources\shader\pudding_shader.glsl"
#include <GameManager.h>
#include "b2Physic.h"
USING_NS_CC;
#define PTM_RATIO  B2Physic::PTM_RATIO

PuddingEffect::PuddingEffect(cocos2d::Texture2D* tex) : _texture(tex)
{
    if (init())
    {
        _ratio = B2Physic::PTM_RATIO;

        _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
        
        b2ParticleSystemDef particleSystemDef;
        particleSystemDef.dampingStrength = 0.2f;
        particleSystemDef.radius = 0.1f;
        _particleSystem = GameManager::Instane()->_b2world->CreateParticleSystem(&particleSystemDef);
        _particleSystem->SetGravityScale(0.4f);
        _particleSystem->SetDensity(1.2f);
    }
}

PuddingEffect::~PuddingEffect()
{
    _renderTexture->release();
}

void PuddingEffect::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    _renderTexture->beginWithClear(0, 0, 0, 0);

    _customCommand.init(RenderQueue::QUEUE_GROUP::GLOBALZ_ZERO);
    _customCommand.func = std::bind(&PuddingEffect::onDraw, this, transform, flags);
    renderer->addCommand(&_customCommand);

    _renderTexture->end();
}
void PuddingEffect::onDraw(const Mat4& transform, uint32_t transformFlags)
{
    glBindVertexArray(_vao);
    _glprogramstate->apply(transform);
    _glprogramstate->setUniformTexture("_texture", _texture->getName());
    GL::blendFunc(_blendFunc.src, _blendFunc.dst);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * _particleSystem->GetParticleCount(), _particleSystem->GetPositionBuffer(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * texCoords.size(), &texCoords[0], GL_STREAM_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    
    //glPolygonMode(GL_FRONT, GL_LINE);

    //int totalParticles = _particleSystem->GetParticleCount();
    //glDrawArrays(GL_POINTS, 0, totalParticles);
    glDrawElements(GL_TRIANGLES, indexs.size(), GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, indexs.size(), GL_UNSIGNED_INT , &indexs[0]);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
   // glPolygonMode(GL_FRONT, GL_FILL);

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, texCoords.size());
    CHECK_GL_ERROR_DEBUG();
}

void PuddingEffect::initData(Type t, cocos2d::Vec2 position, float stride)
{
    auto s = Director::getInstance()->getWinSize();
    texSize = _texture->getContentSize();
    auto center = s / 2;
    auto diff = Size(position) - center;

    b2ParticleGroupDef pd;
    pd.flags = b2_waterParticle/* | b2_viscousParticle | b2_springParticle */ | b2_elasticParticle
        | b2_fixtureContactListenerParticle /*| b2_particleContactListenerParticle*/
        ;
    pd.groupFlags = b2_solidParticleGroup;
    if (position == Vec2::ZERO)
    {
        pd.position.Set(s.width / 2 / PTM_RATIO, s.height / 2 / PTM_RATIO);
    }
    else
    {
        pd.position.Set(position.x/ PTM_RATIO, position.y / PTM_RATIO);
    }
    pd.stride = stride;
    if (t == Type::Box)
    {
        b2PolygonShape shape;
        shape.SetAsBox(texSize.width / PTM_RATIO / 2, texSize.height / PTM_RATIO / 2, b2Vec2(0.0f, 0.0f), 0.0);
        pd.shape = &shape;
        _particleSystem->CreateParticleGroup(pd);
    }
    else
    {
        b2CircleShape shape;
        shape.m_radius = std::min(texSize.width, texSize.height) / 2 / PTM_RATIO;
        pd.shape = &shape;
        _particleSystem->CreateParticleGroup(pd);
    }

    float size = _particleSystem->GetRadius() * 2;
    auto vertsCount = _particleSystem->GetParticleCount();
    auto posVerts = _particleSystem->GetPositionBuffer();
    auto dis = (s - texSize) / 2;
    for (int i = 0; i < vertsCount; i++)
    {
        b2Vec2 vec = posVerts[i];
        texCoords.push_back(Vec2((vec.x * _ratio - dis.width - diff.width) / texSize.width,
          1 - (vec.y * _ratio - dis.height - diff.height) / texSize.height));
    }
    auto steps = _particleSystem->GetSteps();
    indexs.clear();
    for (size_t i = 0, offset = 0; i < steps.size() - 1; i++)
    {
        if (steps[i] == 0)
        {
            continue;
        }
        for (size_t j = 0; j < steps[i] - 1; j++)
        {
            indexs.push_back(j + offset);
            indexs.push_back(j + 1 + offset);
            indexs.push_back(j + steps[i] + offset);
            indexs.push_back(j + 1 + offset);
            indexs.push_back(j + steps[i] + offset);
            indexs.push_back(j + steps[i] + 1 + offset);
        }
        offset += steps[i];
    }
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_vbo1);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs[0]) * indexs.size(), &indexs[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * vertsCount, NULL, GL_STREAM_DRAW);
    // vertex
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), (GLvoid*)(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * vertsCount, NULL, GL_STREAM_DRAW);
   // texcood
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), (GLvoid*)(0));

 /*   _glprogramstate->setVertexAttribPointer("a_position", 2, GL_FLOAT, GL_FALSE, 0, posVerts);
    _glprogramstate->setVertexAttribPointer("a_texCoord", 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), &texCoords[0]);*/

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _glprogramstate->setUniformFloat("u_size", size * _ratio);
}

bool PuddingEffect::init()
{
	std::string vs = pudding_shader_vs;
	std::string fs = pudding_shader_fs;

    auto s = Director::getInstance()->getWinSize();
    _renderTexture = cocos2d::RenderTexture::create(s.width, s.height, Texture2D::PixelFormat::RGBA8888);
    _renderTexture->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _renderTexture->setPosition(Point(s.width / 2, s.height / 2));
    _renderTexture->retain();
    // Change RenderTexture shader
    auto program = GLProgram::createWithByteArrays(pudding_rt_shader_vs, pudding_rt_shader_fs);
    auto programState = GLProgramState::getOrCreateWithGLProgram(program);
    programState->setUniformFloat("u_threshold_discard", 0.15);
    programState->setUniformFloat("u_threshold_border", 0.3);

    _renderTexture->getSprite()->setGLProgramState(programState);
	return initGLProgramState(vs, fs);
}