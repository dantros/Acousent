#include "Speaker.h"

Speaker::Speaker() : Object()
{

	ResetVectors();

    //sound_direction = glm::vec3(0.0f, 0.0f, 1.0f);
	//SoundSource source;
	//this->addAudioSource(source);

	//Model speaker_model(ac::getPath("Resources/Models/Speaker/scene.gltf").string());
	//this->setModel(speaker_model);

}


void Speaker::Update()
{
	transform_model = glm::scale(transform_model, scale_vector);
	transform_model = glm::rotate(transform_model, rotate_angle, rotate_vector);
	transform_model = glm::translate(transform_model, translate_vector);

	position = glm::vec3(transform_model[3][0], transform_model[3][1], transform_model[3][2]);
	sound_direction = glm::normalize(glm::vec3(transform_model * glm::vec4(sound_direction, 0.0f)));

	if (HasAudioSource()) {
		object_Asource->SetPosition(position[0], position[1], position[2]);
		object_Asource->SetDirection(sound_direction[0], sound_direction[1], sound_direction[2]);

	}

	ResetVectors();
	//object_shader->setMat4("model", transform_model);
	//Draw();
}


