#include <SFML/Graphics.hpp>
using namespace sf;

sf::View view;

void setPlayerCoordinateForView(float x, float y) { 
	float tempX = x; float tempY = y;

	//if (x < 320) tempX = 320;
	//if (y < 240) tempY = 240;//верхнюю сторону
	if (y > 640) tempY = 640;//нижнюю стороню.для новой карты

	view.setCenter(tempX, tempY); 
}