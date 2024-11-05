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

  int bounceCooldown = 0;
  int prismCooldown = 0;
  int aliveTime = 0;
  bool isAlive = true;

  unsigned level = 0;

  Projectile(float cLength, float cWidth, int red, int green, int blue,
             float startX, float startY, float cSpeed, float cAngle,
             unsigned cLevel, int cPrismCooldown) {
    length = cLength;
    width = cWidth;
    speed = cSpeed;
    angle = cAngle;
    level = cLevel;
    prismCooldown = cPrismCooldown;

    sprite = sf::RectangleShape(sf::Vector2f(length, width));
    sprite.setOrigin(length / 2, width / 2);
    sprite.setFillColor(sf::Color(red, green, blue));
    float degrees = angle * (180 / PI);

    sprite.rotate(degrees);
    sprite.setPosition(startX, startY);
  }

  bool alive() {

    sf::Vector2f pos = sprite.getPosition();
    if ((pos.x) < 0 || (pos.x) >= WIDTH || (pos.y) < 0 || (pos.y) >= HEIGHT ||
        !isAlive || aliveTime > (0.4 * FPS)) {
      return false;
    }
    return true;
  }

  void display(sf::RenderWindow &window) { window.draw(sprite); }

  void update() {
    float moveX = speed * cos(angle);
    float moveY = speed * sin(angle);
    sprite.move(moveX, moveY);

    if (bounceCooldown > 0) {
      bounceCooldown--;
    }

    if (prismCooldown > 0) {
      prismCooldown--;
    }

    aliveTime++;
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
         float startX, float startY, float cAngle, float cAliveTime) {
    length = cLength;
    width = cWidth;
    angle = cAngle;
    lifeTimer = cAliveTime * FPS;
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

  float qCooldown = 1 * FPS;
  float wCooldown = 5 * FPS;
  float eCooldown = 4 * FPS;
  float rCooldown = 30 * FPS;

  float qTimer = 0;
  float wTimer = 0;
  float eTimer = 0;
  float rTimer = 0;

  bool ePressed = false;

  int eStock = 3;
  int eStockMax = 3;

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

    if (eStock <= 0)
      eText.setString(std::to_string(int(ceil(eTimer / FPS))));
    else
      eText.setString("E " + std::to_string(eStock));

    if (rTimer > 0)
      rText.setString(std::to_string(int(ceil(rTimer / FPS))));
    else
      rText.setString("R");

    window.draw(qText);
    window.draw(wText);
    window.draw(eText);
    window.draw(rText);
  }

  void update(sf::RenderWindow &window, sf::Vector2i mousePos) {
    move();
    checkReflection();
    if (qTimer > 0) {
      qTimer -= 1;
    }
    if (wTimer > 0) {
      wTimer -= 1;
    }

    if (eTimer > 0) {
      eTimer -= 1;
    } else {
      if (eStock < 3) {
        eStock++;
      }
      eTimer = eCooldown;
    }

    if (rTimer > 0) {
      rTimer -= 1;
    }
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

  void abilities(char ability, sf::Vector2i mousePos) {
    switch (ability) {
    case 'Q':
      q(mousePos);
      break;
    case 'W':
      w(mousePos);
      break;
    case 'E':
      e(mousePos);
      break;
    case 'R':
      r();
      break;
    }
  }

private:
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
    Projectile bullet(75, 15, 255, 255, 255, newX, newY, 20, angle, 0, 0);
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
    if (eStock <= 0) {
      return;
    }

    float xDif = mousePos.x - sprite.getPosition().x;
    float yDif = mousePos.y - sprite.getPosition().y;
    float angle = float(atan2(yDif, xDif));

    Mirror mirror(100, 10, 255, 255, 255, mousePos.x, mousePos.y, angle, 15);
    mirrorList.push_back(mirror);

    std::cout << "E\n";

    if (eStock == eStockMax) {
      eTimer = eCooldown;
    }
    eStock--;
  }

  void r() {
    if (rTimer > 0) {
      return;
    }

    std::cout << "R\n";
    float rDistance = 250;

    for (int i = 0; i < 18; i++) {
      float angle = i * 20;
      float radians = angle * PI / 180;

      float newX = sprite.getPosition().x + rDistance * cos(radians);
      float newY = sprite.getPosition().y + rDistance * sin(radians);

      Mirror mirror(75, 10, 255, 255, 255, newX, newY, radians, 5);
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

      if (projectileList[i].bounceCooldown > 0) {
        continue;
      }
      for (int j = 0; j < mirrorList.size(); j++) {
        Projectile bullet = projectileList[i];
        Mirror mirror = mirrorList[j];

        float bulletX = bullet.sprite.getPosition().x;
        float bulletY = bullet.sprite.getPosition().y;

        sf::Vector2f bullet1 = {
            bulletX + float(bullet.length / 2) * float(cos(bullet.angle)),
            bulletY + float(bullet.length / 2) * float(sin(bullet.angle))};

        sf::Vector2f bullet2 = {
            bulletX - float(bullet.length / 2) * float(cos(bullet.angle)),
            bulletY - float(bullet.length / 2) * float(sin(bullet.angle))};

        float mirrorX = mirror.sprite.getPosition().x;
        float mirrorY = mirror.sprite.getPosition().y;

        sf::Vector2f mirror1 = {
            mirrorX +
                float(mirror.length / 2) * float(cos(mirror.angle - (PI / 2))),
            mirrorY +
                float(mirror.length / 2) * float(sin(mirror.angle - (PI / 2)))};

        sf::Vector2f mirror2 = {
            mirrorX -
                float(mirror.length / 2) * float(cos(mirror.angle - (PI / 2))),
            mirrorY -
                float(mirror.length / 2) * float(sin(mirror.angle - (PI / 2)))};

        if (doIntersect(bullet1, bullet2, mirror1, mirror2)) {
          std::cout << "Intersect\n";
          float angleDif = bullet.angle - mirror.angle;
          float newAngle = mirror.angle - angleDif + PI;

          projectileList[i].angle = newAngle;
          projectileList[i].sprite.setRotation(newAngle * 180 / PI);

          projectileList[i].bounceCooldown = 3;
          projectileList[i].aliveTime = -.3 * FPS;
        }
      }

      for (int j = 0; j < prismList.size(); j++) {
        float xDif = prismList[j].sprite.getPosition().x -
                     projectileList[i].sprite.getPosition().x;
        float yDif = prismList[j].sprite.getPosition().y -
                     projectileList[i].sprite.getPosition().y;

        float distance = sqrt(xDif * xDif + yDif * yDif);

        if (distance <= prismList[j].size &&
            projectileList[i].prismCooldown <= 0) {

          projectileList[i].isAlive = false;
          projectileList[i].prismCooldown = 3;

          float prevLevel = projectileList[i].level;
          if (prevLevel >= 6 || projectileList.size() >= 100) {
            continue;
          }

          float sX = projectileList[i].sprite.getPosition().x;
          float sY = projectileList[i].sprite.getPosition().y;
          float sAngle = projectileList[i].angle;
          float sSize = 0;
          float nLength = 50 - (10 * (prevLevel + 1));

          Projectile bulletRed(50, 10, 255, 0, 0,
                               sX + sSize * cos(sAngle - (PI / 8)),
                               sY + sSize * sin(sAngle - (PI / 8)), 20,
                               sAngle - (PI / 8), prevLevel + 1, 3);
          Projectile bulletBlue(50, 10, 0, 0, 255,
                                sX + sSize * cos(sAngle + (PI / 8)),
                                sY + sSize * sin(sAngle + (PI / 8)), 20,
                                sAngle + (PI / 8), prevLevel + 1, 3);

          Projectile bulletGreen(25, 10, 0, 255, 0, sX + sSize * cos(sAngle),
                                 sY + sSize * sin(sAngle), 20, sAngle,
                                 prevLevel + 1, 3);

          projectileList.push_back(bulletRed);
          projectileList.push_back(bulletBlue);
          projectileList.push_back(bulletGreen);
        }
      }
    }
  }

  bool onSegment(sf::Vector2f p, sf::Vector2f q, sf::Vector2f r) {
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
      return true;

    return false;
  }

  int orientation(sf::Vector2f p, sf::Vector2f q, sf::Vector2f r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0)
      return 0; // collinear

    return (val > 0) ? 1 : 2; // clock or counterclock wise
  }

  // The main function that returns true if line segment 'p1q1'
  // and 'p2q2' intersect.
  bool doIntersect(sf::Vector2f p1, sf::Vector2f q1, sf::Vector2f p2,
                   sf::Vector2f q2) {
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
      return true;

    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1))
      return true;

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1))
      return true;

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2))
      return true;

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2))
      return true;

    return false; // Doesn't fall in any of the above cases
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

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Q) {
          sf::Vector2i mousePos = sf::Mouse::getPosition(window);
          player.abilities('Q', mousePos);
        }
        if (event.key.code == sf::Keyboard::W) {
          sf::Vector2i mousePos = sf::Mouse::getPosition(window);
          player.abilities('W', mousePos);
        }
        if (event.key.code == sf::Keyboard::F) {
          sf::Vector2i mousePos = sf::Mouse::getPosition(window);
          player.abilities('E', mousePos);
        }
        if (event.key.code == sf::Keyboard::P) {
          player.abilities('R', {});
        }
      }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
      sf::Vector2i mousePos = sf::Mouse::getPosition(window);
      if (0 <= mousePos.x && mousePos.x < WIDTH && 0 <= mousePos.y &&
          mousePos.y < HEIGHT)
        player.setMovePoint(mousePos.x, mousePos.y);
    }

    window.clear();
    window.draw(bg);
    player.update(window, sf::Mouse::getPosition(window));
    player.display(window);

    window.display();
  }

  return 0;
}
