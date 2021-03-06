#include "Sprite.h"
#include "Vertex.h"
#include "Globals.h"
#include "SpriteSheet.h"
#include "Frame.h"
#include "../SKTGameProject/SingletonClasses.h"

Sprite::Sprite(): m_fAlpha(1.0f), m_Index(0)
{
	m_matMVP.SetOrthographic(-float(Globals::screenWidth) / 2, float(Globals::screenWidth / 2),
	                         float(Globals::screenHeight / 2), -float(Globals::screenHeight / 2), 0.1f, 40.0f);
}

Sprite::~Sprite()
{
}

void Sprite::SetFrame(Frame* frame)
{
	m_pTexture = ResourceMgr->GetSpriteSheetById(frame->GetSpriteSheetId());
	m_Index = frame->GetIndex();
}

void Sprite::SetOpacity(float alpha)
{
	m_fAlpha = alpha;
}

void Sprite::Render()
{
	glUseProgram(m_pShaders->program);
	glBindBuffer(GL_ARRAY_BUFFER, m_pModel->GetVboID());
	glBindTexture(GL_TEXTURE_2D, m_pTexture->GetTextureID());

	// Load model
	GLuint posLocation = m_pShaders->locationAttributePos;
	if (posLocation != -1)
	{
		glEnableVertexAttribArray(posLocation);
		glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
	}

	// Load UV of first tile in spritesheet
	GLuint uvLocation = m_pShaders->locationAttributeUV;
	if (uvLocation != -1)
	{
		glEnableVertexAttribArray(uvLocation);
		glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
	}

	// Load texture 
	GLuint iTexLocation = m_pShaders->locationUniformTex;
	if (iTexLocation != -1)
	{
		glUniform1i(iTexLocation, 0);
	}

	// Load MVP matrix
	GLuint mvpLocation = m_pShaders->locationUniformMVP;
	if (mvpLocation != -1)
	{
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &(m_matMVP.m[0][0]));
	}

	// Load delta vector (to calculate UV of actual tile in spritesheet)
	GLuint deltaLocation = m_pShaders->locationUniformDelta;
	if (deltaLocation != -1)
	{
		Vector2 vec2Delta;
		vec2Delta = static_cast<SpriteSheet*>(m_pTexture)->GetDeltaByIndex(m_Index);
		float delta[2];
		delta[0] = vec2Delta.x;
		delta[1] = vec2Delta.y;
		glUniform2fv(deltaLocation, 1, delta);
	}

	// Load alpha value
	GLuint alphaLocation = m_pShaders->locationUniformAlpha;
	if (alphaLocation != -1)
	{
		glUniform1f(alphaLocation, m_fAlpha);
	}

	// Draw sprite
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Sprite::SetRenderInfo(Vector2 position, bool isReversed, Vector2 vec2Scale)
{
	Matrix matModel, matProjection, matScale, matTranslation, matRotation;
	matTranslation.SetTranslation(position.x, position.y, 0);
	matScale.SetScale(vec2Scale.x, vec2Scale.y, 1);
	matRotation.SetIdentity();
	if (isReversed)
	{
		matRotation.SetRotationY(Radians(180));
	}

	matModel = matScale * matRotation * matTranslation;
	matProjection.SetOrthographic(-float(Globals::screenWidth) / 2, float(Globals::screenWidth / 2),
	                              float(Globals::screenHeight / 2), -float(Globals::screenHeight / 2), 0.1f, 40.0f);
	m_matMVP = matModel * matProjection;
}

void Sprite::SetIndex(int index)
{
	m_Index = index;
}
