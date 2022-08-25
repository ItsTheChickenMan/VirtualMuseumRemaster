// audio manager (sfml does most of the work, but an abstraction of SFML's methods is nice)

#ifndef VMR_AUDIO_H
#define VMR_AUDIO_H

// includes //
#define SFML_STATIC
#include <SFML/Audio.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

// methods //
bool loadSoundFile(std::string filename, std::string key);
bool loadSoundFileBatch(std::string filenames[], std::string keys[], uint32_t count);

sf::Sound* createSound(std::string key);
void playSound(std::string key);
void playSound(std::string key, glm::vec3 position);
void updateSounds();

void updateListener(glm::vec3 position, glm::vec3 forward);

void playBackgroundMusic(std::string filename, float volume, bool loop);
void playBackgroundMusic(std::string filename, float volume);
void playBackgroundMusic(std::string filename);
void setBackgroundMusicVolume(float volume);
void setBackgroundMusicLoop(bool loop);

sf::Vector3<float> glmVecToSFML(glm::vec3 vec);

#endif