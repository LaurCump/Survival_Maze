#include "lab_m1/Tema2/Tema2_h/player2.h"

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace m1;

Player2::Player2(glm::vec3 pos, glm::vec3 size, glm::vec3 color, float moveSpeed, int ammo)
{
	this->pos = pos;
	this->size = size;
	this->color = color;
	this->moveSpeed = moveSpeed;
	this->ammo = ammo;
	lifes = totalLifes;
	cooldownTimer = 0;

	for (int i = 0; i < ammo; ++i) 
	{
		bullets.push_back(new Bullet2());
	}
	enabled = true;
	lookAngle = 0;
}

Player2::~Player2()
{
	for each (Bullet2 *b in bullets)
	{
		delete b;
	}
	bullets.clear();
}

void Player2::UpdateBullets(float deltaTimeSeconds) 
{
	for each (Bullet2 *b in bullets) 
	{
		// actualizare viata glontului
		if (!b->available) 
		{
			if (b->lifeTimer <= 0) 
			{
				b->Reset();
				continue;
			}

			// actualizare pozitia glontului
			glm::vec3 distance{b->direction.x * deltaTimeSeconds * b->speed, b->direction.y * deltaTimeSeconds * b->speed, b->direction.z * deltaTimeSeconds * b->speed};

			b->pos.x += distance.x;
			b->pos.y += distance.y;
			b->pos.z += distance.z;

			b->lifeTimer -= deltaTimeSeconds;
		}
	}

	cooldownTimer = glm::max(cooldownTimer - deltaTimeSeconds, 0.0f);
}


void Player2::RotateFirstPerson_OY(float angle)
{
	glm::vec4 tmp_vec4 = glm::vec4(forward, 1.0);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
	tmp_vec4 = model * tmp_vec4;
	forward = glm::normalize(glm::vec3(tmp_vec4));

	tmp_vec4 = glm::vec4(right, 1.0);
	model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
	tmp_vec4 = model * tmp_vec4;
	right = glm::normalize(glm::vec3(tmp_vec4));

	up = glm::cross(right, forward);
}

void Player2::RotateFirstPerson_OZ(float angle)
{
	glm::vec4 tmp_vec4 = glm::vec4(right, 1.0);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, forward);
	tmp_vec4 = model * tmp_vec4;
	right = glm::normalize(glm::vec3(tmp_vec4));

	up = glm::cross(right, forward);
}


void Player2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY, WindowObject* window, float sensitivity)
{
	if (!enabled) return;

	RotateFirstPerson_OY(-(float)deltaX * sensitivity);
}

void Player2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods, WindowObject* window)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT))
	{
		enabled = true;
	}
}

void Player2::OnKeyPress(int key, int mods, WindowObject* window)
{
	if (key == GLFW_KEY_F1)
	{
		enabled = false;
	}
}

