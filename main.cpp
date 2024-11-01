#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <iostream>
#include <string>

int FPS = 60;
int WIDTH = 500;
int HEIGHT = 500;

class MoveParticle {
public:
  float size;
  int rgb[3];
  sf::Vector2i pos;

  MoveParticle(float cSize, int red, int green, int blue, float startX,
               float startY) {
    rgb[0] = red;
    rgb[1] = green;
    rgb[2] = blue;
    size = cSize;
    pos.x = startX;
    pos.y = startY;
  }

  void display(sf::RenderWindow &window) {
    size--;
    sf::CircleShape sprite(size);
    sprite.setFillColor(sf::Color::Transparent);
    sprite.setOutlineThickness(3);
    sprite.setOutlineColor(sf::Color(rgb[0], rgb[1], rgb[2]));

    sprite.setPosition(pos.x + size / 2, pos.y + size / 2);
    window.draw(sprite);
  }
  bool alive() { return size > 0; }
};

class Projectile {
  sf::CircleShape sprite;
  float speed;
  float size;
  float angle;

public:
  Projectile(float cSize, int red, int green, int blue, float startX,
             float startY, float cSpeed, float cAngle) {
    sprite = sf::CircleShape(cSize);
    sprite.setFillColor(sf::Color(red, green, blue));
    sprite.setPosition(startX + cSize / 2, startY + cSize / 2);

    size = cSize;
    speed = cSpeed;
    angle = cAngle;
  }

  bool alive() {
    sf::Vector2f pos = sprite.getPosition();
    if ((pos.x + size / 2) < 0 or (pos.x + size / 2) >= WIDTH or
        (pos.y + size / 2) < 0 or (pos.y + size / 2) >= HEIGHT) {
      return false;
    }
    return true;
  }

  void display(sf::RenderWindow &window) { window.draw(sprite); }

  void update() {
    float moveX = speed * cos(angle);
    float moveY = speed * sin(angle);
    sprite.move(moveX, moveY);
  }
};

class Player {
public:
  sf::Font font;
  sf::Text qText;

  sf::CircleShape sprite;
  int size;
  float speed;
  float movePoint[2];
  bool moving = false;

  float qCooldown = 2 * FPS;
  float wCooldown = 1 * FPS;

  float qTimer = 0;
  float wTimer = 0;

  std::vector<Projectile> projectileList;
  std::vector<sf::RectangleShape> mirrorList;
  std::vector<MoveParticle> moveParticleList;

  Player(int cSize, int red, int green, int blue, float startX, float startY,
         float cSpeed, sf::Font cFont) {

    font = cFont;
    qText.setFont(font);
    qText.setCharacterSize(30);
    qText.setFillColor(sf::Color::Black);

    sprite = sf::CircleShape(cSize);
    sprite.setFillColor(sf::Color(red, green, blue));
    sprite.setPosition(startX, startY);
    size = cSize;
    movePoint[0] = startX;
    movePoint[1] = startY;
    speed = cSpeed;
  }

  void display(sf::RenderWindow &window) {
    window.draw(sprite);

    for (int i = projectileList.size() - 1; i >= 0; i--) {
      if (!projectileList[i].alive()) {
        projectileList.erase(projectileList.begin() + i);
        continue;
      }
      projectileList[i].display(window);
      projectileList[i].update();
    }

    for (int i = moveParticleList.size() - 1; i >= 0; i--) {
      if (!moveParticleList[i].alive()) {
        moveParticleList.erase(moveParticleList.begin() + i);
        continue;
      }
      moveParticleList[i].display(window);
    }

    for (int i = mirrorList.size() - 1; i >= 0; i--) {
      window.draw(mirrorList[i]);
    }

    if (qTimer > 0)
      qText.setString(std::to_string(int(ceil(qTimer / FPS))));
    else
      qText.setString("Q");

    window.draw(qText);
  }

  void update(sf::Vector2i mousePos) {
    move();
    abilities(mousePos);
  }

  void setMovePoint(float x, float y) {
    moving = true;
    movePoint[0] = x - size;
    movePoint[1] = y - size;
    MoveParticle moveParticle(30, 46, 204, 113, movePoint[0] + size / 2,
                              movePoint[1] + size / 2);
    if (moveParticleList.size() == 0) {
      moveParticleList.push_back(moveParticle);
    } else {
      moveParticleList[0] = (moveParticle);
    }
  }

private:
  void abilities(sf::Vector2i mousePos) {
    if (qTimer > 0) {
      qTimer -= 1;
    }
    if (wTimer > 0) {
      wTimer -= 1;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
      q(mousePos);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      w(mousePos);
    }

    sf::Text text;
  }

  void q(sf::Vector2i mousePos) {
    if (qTimer <= 0) {
      std::cout << "Q\n";
      qTimer = qCooldown;
      return;
    }

    float xDif = mousePos.x - sprite.getPosition().x + size / 2;
    float yDif = mousePos.y - sprite.getPosition().y + size / 2;

    float angle = float(atan2(yDif, xDif));

    for (int i = 0; i < 5; i++) {
      float newX =
          sprite.getPosition().x + (i * size / 2) * cos(angle) + size / 2;
      float newY =
          sprite.getPosition().y + (i * size / 2) * sin(angle) + size / 2;
      Projectile bullet(10, 255, 255, 255, newX, newY, 20, angle);
      projectileList.push_back(bullet);
    }
  }

  void w(sf::Vector2i mousePos) {
    if (wTimer <= 0) {
      std::cout << "W\n";
      wTimer = wCooldown;
      return;
    }
  }

  void move() {
    if (!moving)
      return;

    float xDif = movePoint[0] - sprite.getPosition().x + size / 2;
    float yDif = movePoint[1] - sprite.getPosition().y + size / 2;

    float distance = sqrt((xDif * xDif) + (yDif * yDif));
    float angle = float(atan2(yDif, xDif));

    if (distance < speed) {
      moving = false;
      sprite.setPosition(movePoint[0] + size / 2, movePoint[1] + size / 2);
    } else {
      float moveX = speed * cos(angle);
      float moveY = speed * sin(angle);
      sprite.move(moveX, moveY);
    }
  }
};

int main() {
  sf::Font font;
  if (!font.loadFromFile("./Lemon.otf")) {
    return 0;
  }

  std::vector<Projectile> projectileList;
  Projectile testBullet(10, 0, 0, 0, WIDTH / 2, HEIGHT / 2, 5, 5);
  projectileList.push_back(testBullet);

  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Test");
  window.setFramerateLimit(FPS);

  sf::RectangleShape bg(sf::Vector2f(WIDTH, HEIGHT));
  bg.setFillColor(sf::Color(189, 195, 199));

  int playerRadius = 25;
  Player player(playerRadius, 231, 76, 60, WIDTH / 2 - playerRadius,
                HEIGHT / 2 - playerRadius, 4, font);

  while (window.isOpen()) {

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
      sf::Vector2i mousePos = sf::Mouse::getPosition(window);
      if (0 <= mousePos.x and mousePos.x < WIDTH and 0 <= mousePos.y and
          mousePos.y < HEIGHT)
        player.setMovePoint(mousePos.x, mousePos.y);
    }

    window.clear();
    window.draw(bg);
    player.update(sf::Mouse::getPosition(window));
    player.display(window);

    window.display();
  }

  return 0;
}
