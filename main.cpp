#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <iostream>
#include <string>

int FPS = 60;
int WIDTH = 800;
int HEIGHT = 800;
float PI = atan(1) * 4;

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
    sprite.setOrigin(size, size);
    sprite.setFillColor(sf::Color::Transparent);
    sprite.setOutlineThickness(3);
    sprite.setOutlineColor(sf::Color(rgb[0], rgb[1], rgb[2]));

    sprite.setPosition(pos.x, pos.y);
    window.draw(sprite);
  }
  bool alive() { return size > 0; }
};

class Projectile {
public:
  sf::RectangleShape sprite;
  float speed;
  float length;
  float width;
  float angle;

  Projectile(float cLength, float cWidth, int red, int green, int blue,
             float startX, float startY, float cSpeed, float cAngle) {
    length = cLength;
    width = cWidth;
    speed = cSpeed;
    angle = cAngle;

    sprite = sf::RectangleShape(sf::Vector2f(length, width));
    sprite.setOrigin(length / 2, width / 2);
    sprite.setFillColor(sf::Color(red, green, blue));
    float degrees = angle * (180 / PI);

    sprite.rotate(degrees);
    sprite.setPosition(startX, startY);
  }

  bool alive() {
    sf::Vector2f pos = sprite.getPosition();
    if ((pos.x) < 0 or (pos.x) >= WIDTH or (pos.y) < 0 or (pos.y) >= HEIGHT) {
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

class Mirror {
  float lifeTimer;

public:
  sf::RectangleShape sprite;
  float length;
  float width;
  float angle;

  Mirror(float cLength, float cWidth, int red, int green, int blue,
         float startX, float startY, float cAngle) {
    length = cLength;
    width = cWidth;
    angle = cAngle;
    lifeTimer = 5 * FPS;
    float degrees = angle * (180 / PI) + 90;

    sprite = sf::RectangleShape(sf::Vector2f(length, width));
    sprite.setOrigin(length / 2, width / 2);
    sprite.setFillColor(sf::Color(red, green, blue));
    sprite.setPosition(startX, startY);

    sprite.rotate(degrees);
  }

  bool alive() {
    if (lifeTimer < 0) {
      return false;
    }
    return true;
  }

  void display(sf::RenderWindow &window) { window.draw(sprite); }

  void update() { lifeTimer--; }
};

class Prism {
  float lifeTimer;

public:
  sf::CircleShape sprite;
  float size;
  float angle;

  Prism(float cSize, int red, int green, int blue, float startX, float startY,
        float cAngle) {
    size = cSize;
    angle = cAngle;
    lifeTimer = 5 * FPS;
    float degrees = angle * (180 / PI) + 30;

    sprite = sf::CircleShape(size, 3);
    sprite.setOrigin(size, size);
    sprite.setFillColor(sf::Color(red, green, blue));

    sprite.setPosition(startX, startY);
    sprite.rotate(degrees);
    // sprite.rotate(30);
  }

  bool alive() {
    if (lifeTimer < 0) {
      return false;
    }
    return true;
  }

  void display(sf::RenderWindow &window) { window.draw(sprite); }

  void update() {
    lifeTimer--;
    // sprite.rotate(.5);
  }
};

class Player {
public:
  sf::Font font;
  sf::Text qText;
  sf::Text wText;
  sf::Text eText;
  sf::Text rText;

  sf::CircleShape sprite;
  int size;
  float speed;
  float movePoint[2];
  bool moving = false;

  float qCooldown = 2 * FPS;
  float wCooldown = 5 * FPS;
  float eCooldown = 1 * FPS;
  float rCooldown = 30 * FPS;

  float qTimer = 0;
  float wTimer = 0;
  float eTimer = 0;
  float rTimer = 0;

  std::vector<Projectile> projectileList;
  std::vector<Mirror> mirrorList;
  std::vector<Prism> prismList;
  std::vector<MoveParticle> moveParticleList;

  Player(int cSize, int red, int green, int blue, float startX, float startY,
         float cSpeed, sf::Font cFont) {

    font = cFont;
    qText.setFont(font);
    qText.setCharacterSize(50);
    qText.setFillColor(sf::Color::Black);
    qText.setPosition(WIDTH * (1.0 / 8) - 25, HEIGHT - 75);

    wText.setFont(font);
    wText.setCharacterSize(50);
    wText.setFillColor(sf::Color::Black);
    wText.setPosition(WIDTH * (3.0 / 8) - 25, HEIGHT - 75);

    eText.setFont(font);
    eText.setCharacterSize(50);
    eText.setFillColor(sf::Color::Black);
    eText.setPosition(WIDTH * (5.0 / 8) - 25, HEIGHT - 75);

    rText.setFont(font);
    rText.setCharacterSize(50);
    rText.setFillColor(sf::Color::Black);
    rText.setPosition(WIDTH * (7.0 / 8) - 25, HEIGHT - 75);

    size = cSize;
    movePoint[0] = startX;
    movePoint[1] = startY;
    speed = cSpeed;

    sprite = sf::CircleShape(size);
    sprite.setOrigin(size, size);
    sprite.setFillColor(sf::Color(red, green, blue));
    sprite.setPosition(movePoint[0], movePoint[1]);
  }

  void display(sf::RenderWindow &window) {
    for (int i = moveParticleList.size() - 1; i >= 0; i--) {
      if (!moveParticleList[i].alive()) {
        moveParticleList.erase(moveParticleList.begin() + i);
        continue;
      }
      moveParticleList[i].display(window);
    }

    for (int i = projectileList.size() - 1; i >= 0; i--) {
      if (!projectileList[i].alive()) {
        projectileList.erase(projectileList.begin() + i);
        continue;
      }
      projectileList[i].display(window);
      projectileList[i].update();
    }

    for (int i = mirrorList.size() - 1; i >= 0; i--) {
      if (!mirrorList[i].alive()) {
        mirrorList.erase(mirrorList.begin() + i);
        continue;
      }

      mirrorList[i].update();
      mirrorList[i].display(window);
    }

    for (int i = prismList.size() - 1; i >= 0; i--) {
      if (!prismList[i].alive()) {
        prismList.erase(prismList.begin() + i);
        continue;
      }

      prismList[i].update();
      prismList[i].display(window);
    }

    window.draw(sprite);

    if (qTimer > 0)
      qText.setString(std::to_string(int(ceil(qTimer / FPS))));
    else
      qText.setString("Q");

    if (wTimer > 0)
      wText.setString(std::to_string(int(ceil(wTimer / FPS))));
    else
      wText.setString("W");

    if (eTimer > 0)
      eText.setString(std::to_string(int(ceil(eTimer / FPS))));
    else
      eText.setString("E");

    if (rTimer > 0)
      rText.setString(std::to_string(int(ceil(rTimer / FPS))));
    else
      rText.setString("R");

    window.draw(qText);
    window.draw(wText);
    window.draw(eText);
    window.draw(rText);
  }

  void update(sf::Vector2i mousePos) {
    move();
    abilities(mousePos);
  }

  void setMovePoint(float x, float y) {
    moving = true;
    movePoint[0] = x;
    movePoint[1] = y;
    MoveParticle moveParticle(30, 46, 204, 113, movePoint[0], movePoint[1]);
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

    if (eTimer > 0) {
      eTimer -= 1;
    }

    if (rTimer > 0) {
      rTimer -= 1;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
      q(mousePos);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      w(mousePos);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
      e(mousePos);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
      r();
    }

    sf::Text text;
  }

  void q(sf::Vector2i mousePos) {
    if (qTimer > 0) {
      return;
    }

    std::cout << "Q\n";
    float xDif = mousePos.x - sprite.getPosition().x;
    float yDif = mousePos.y - sprite.getPosition().y;

    float angle = float(atan2(yDif, xDif));

    float newX = sprite.getPosition().x;
    float newY = sprite.getPosition().y;
    Projectile bullet(50, 10, 255, 255, 255, newX, newY, 20, angle);
    projectileList.push_back(bullet);

    qTimer = qCooldown;
  }

  void w(sf::Vector2i mousePos) {
    if (wTimer > 0) {
      return;
    }

    std::cout << "W\n";
    float xDif = mousePos.x - sprite.getPosition().x;
    float yDif = mousePos.y - sprite.getPosition().y;
    float angle = float(atan2(yDif, xDif));

    Prism prism(30, 255, 255, 255, mousePos.x, mousePos.y, angle);
    prismList.push_back(prism);

    std::cout << "W\n";

    wTimer = wCooldown;
  }

  void e(sf::Vector2i mousePos) {
    if (eTimer > 0) {
      return;
    }

    float xDif = mousePos.x - sprite.getPosition().x;
    float yDif = mousePos.y - sprite.getPosition().y;
    float angle = float(atan2(yDif, xDif));

    Mirror mirror(75, 10, 255, 255, 255, mousePos.x, mousePos.y, angle);
    mirrorList.push_back(mirror);

    std::cout << "E\n";

    eTimer = eCooldown;
  }

  void r() {
    if (rTimer > 0) {
      return;
    }

    std::cout << "R\n";
    float rDistance = 300;

    for (int i = 0; i < 18; i++) {
      float angle = i * 20;
      float radians = angle * PI / 180;

      float newX = sprite.getPosition().x + rDistance * cos(radians);
      float newY = sprite.getPosition().y + rDistance * sin(radians);

      Mirror mirror(75, 10, 255, 255, 255, newX, newY, radians);
      mirrorList.push_back(mirror);
    }

    rTimer = rCooldown;
  }

  void move() {
    if (!moving)
      return;

    float xDif = movePoint[0] - sprite.getPosition().x;
    float yDif = movePoint[1] - sprite.getPosition().y;

    float distance = sqrt((xDif * xDif) + (yDif * yDif));
    float angle = float(atan2(yDif, xDif));

    if (distance < speed) {
      moving = false;
      sprite.setPosition(movePoint[0], movePoint[1]);
    } else {
      float moveX = speed * cos(angle);
      float moveY = speed * sin(angle);
      sprite.move(moveX, moveY);
    }
  }

  void checkReflection() {
    for (int i = 0; i < projectileList.size(); i++) {
      for (int j = 0; j < mirrorList.size(); j++) {
        Projectile bullet = projectileList[i];
        Mirror mirror = mirrorList[j];

        float bulletX = bullet.sprite.getPosition().x;
        float bulletY = bullet.sprite.getPosition().y;

        float bulletNewX = 0;
        float bulletNewY = 0;

        float mirrorX = mirror.sprite.getPosition().x;
        float mirrorY = mirror.sprite.getPosition().y;

        if (bulletX < mirrorX && bulletNewX > mirrorX) {
        }
      }
    }
  }
};

int main() {
  sf::Font font;
  if (!font.loadFromFile("./Lemon.otf")) {
    return 0;
  }

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
