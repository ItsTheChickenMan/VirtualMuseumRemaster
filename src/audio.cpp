// audio manager (sfml does most of the work, but an abstraction of SFML's methods is nice)
#include <audio.h>

#include <cstdint>

#include <map>
#include <string>
#include <vector>

// sound buffer manager
std::map<std::string, sf::SoundBuffer*> bufferManager;

// sound manager
std::vector<sf::Sound*> soundManager;

// looping background music
sf::Music backgroundMusic;

// buffer management //

// load a sound file from a filename and a key to reference in playSound
bool loadSoundFile(std::string filename, std::string key){
	// create buffer object
	sf::SoundBuffer* buffer = new sf::SoundBuffer();
	
	// load from filename
	if(!buffer->loadFromFile(filename)){
		return false;
	}
	
	// add to manager
	bufferManager[key] = buffer;
	
	return true;
}

// load a batch of sound files at once
bool loadSoundFileBatch(std::string filenames[], std::string keys[], uint32_t count){
	for(uint32_t i = 0; i < count; i++){
		if(!loadSoundFile(filenames[i], keys[i])){
			return false;
		}
	}
	
	return true;
}

// sound management // 

// create sound (delete when done)
sf::Sound* createSound(std::string key){
	sf::SoundBuffer* buffer= bufferManager[key];
	
	if(buffer == NULL){
		printf("Invalid key for sound: %s\n", key.c_str());
		return NULL;
	}
	
	sf::Sound* sound = new sf::Sound(*buffer);
	
	return sound;
}

// create and play a sound
void playSound(std::string key){
	sf::Sound* sound = createSound(key);
	
	if(sound == NULL) return;
	
	sound->play();
	
	soundManager.push_back(sound);
}

// create and play a sound at a certain position
// should be used if the position is static (set once and never changed)
void playSound(std::string key, glm::vec3 position){
	sf::Sound* sound = createSound(key);
	
	if(sound == NULL) return;
	
	// set relative and assign position
	sound->setRelativeToListener(true);
	sound->setPosition(glmVecToSFML(position));
	
	sound->play();
	
	soundManager.push_back(sound);
}

// TODO: create a method to create and play a sound, binding it to a certain position constantly

// update sound positions and delete any stopped sounds
void updateSounds(){
	for(uint32_t i = 0; i < (uint32_t)soundManager.size(); i++){
		sf::Sound* sound = soundManager[i];
		
		// delete stopped sounds
		if(sound->getStatus() == sf::Sound::Stopped){
			delete sound;
			
			soundManager.erase(soundManager.begin()+i);
			i--;
		}
	}
}

// listener //

void updateListener(glm::vec3 position, glm::vec3 forward){
	sf::Listener::setPosition(glmVecToSFML(position));
	sf::Listener::setDirection(glmVecToSFML(forward));
}

// music //
void playBackgroundMusic(std::string filename, float volume){
	if(backgroundMusic.getStatus() != sf::Music::Stopped){
		backgroundMusic.stop();
	}
	
	if(!backgroundMusic.openFromFile(filename)){
		printf("Invalid path to music source file\n");
		return;
	}
	
	backgroundMusic.setLoop(true);
	backgroundMusic.setVolume(volume);
	
	backgroundMusic.play();
}

void playBackgroundMusic(std::string filename){
	playBackgroundMusic(filename, 100.f);
}

void setBackgroundMusicVolume(float volume){
	backgroundMusic.setVolume(volume);
}

// utils // 

// convert glm 3 component vector to sfml 3 component vector
sf::Vector3<float> glmVecToSFML(glm::vec3 vec){
	return sf::Vector3(vec.x, vec.y, vec.z);
}