#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <cstdlib>
#include <iostream>

// Constants
const int GAME_WIDTH = 1000;
const int GAME_HEIGHT = static_cast<int>(GAME_WIDTH * 0.5555);
const int BALL_DIAMETER = 20;
const int PADDLE_WIDTH = 25;
const int PADDLE_HEIGHT = 100;
const int PADDLE_SPEED = 10;
const int BALL_INITIAL_SPEED = 2;

// Score class
class Score {
public:
    int player1 = 0;
    int player2 = 0;

    void draw(sf::RenderWindow &window) {
        sf::Font font;
        if (!font.loadFromFile("consolas.ttf")) {
            // Handle error
        }
        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString(std::to_string(player1) + "  " + std::to_string(player2));
        scoreText.setCharacterSize(60);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(GAME_WIDTH / 2 - 85, 20);
        window.draw(scoreText);
    }
};

// Paddle class
// Paddle class
class Paddle : public sf::RectangleShape {
public:
    int id;
    int yVelocity = 0;

    Paddle(int x, int y, int id) : id(id) {
        this->setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
        this->setPosition(x, y);
    }

    void move() {
        sf::RectangleShape::move(0, yVelocity);
    }

    void setYDirection(int yDirection) {
        yVelocity = yDirection * PADDLE_SPEED;
    }

    void keyPress(sf::Keyboard::Key key) {
        if (id == 1) {
            if (key == sf::Keyboard::W)
                setYDirection(-1);
            if (key == sf::Keyboard::S)
                setYDirection(1);
        } else if (id == 2) {
            if (key == sf::Keyboard::Up)
                setYDirection(-1);
            if (key == sf::Keyboard::Down)
                setYDirection(1);
        }
    }

    void keyRelease(sf::Keyboard::Key key) {
        if (id == 1) {
            if (key == sf::Keyboard::W || key == sf::Keyboard::S)
                setYDirection(0);
        } else if (id == 2) {
            if (key == sf::Keyboard::Up || key == sf::Keyboard::Down)
                setYDirection(0);
        }
    }
};

// Ball class
class Ball : public sf::CircleShape {
public:
    int xVelocity;
    int yVelocity;

    Ball(int x, int y) {
        this->setRadius(BALL_DIAMETER / 2);
        this->setPosition(x, y);
        setRandomDirection();
    }

    void move() {
        sf::CircleShape::move(xVelocity, yVelocity);
    }

    void setRandomDirection() {
        xVelocity = (rand() % 2 == 0 ? -1 : 1) * BALL_INITIAL_SPEED;
        yVelocity = (rand() % 2 == 0 ? -1 : 1) * BALL_INITIAL_SPEED;
    }

    void reverseX() {
        xVelocity = -xVelocity;
    }

    void reverseY() {
        yVelocity = -yVelocity;
    }
};



void checkCollision(Ball &ball, Paddle &paddle1, Paddle &paddle2, Score &score) {
    // Bounce ball off top & bottom window edges
    if (ball.getPosition().y <= 0 || ball.getPosition().y >= GAME_HEIGHT - BALL_DIAMETER) {
        ball.reverseY();
    }

    // Bounce ball off paddles
    if (ball.getGlobalBounds().intersects(paddle1.getGlobalBounds())) {
        ball.reverseX();
        ball.xVelocity++;
        if (ball.yVelocity > 0)
            ball.yVelocity++;
        else
            ball.yVelocity--;
    }
    if (ball.getGlobalBounds().intersects(paddle2.getGlobalBounds())) {
        ball.reverseX();
        ball.xVelocity++;
        if (ball.yVelocity > 0)
            ball.yVelocity++;
        else
            ball.yVelocity--;
    }

    // Stops paddles at window edges
    if (paddle1.getPosition().y <= 0)
        paddle1.setPosition(paddle1.getPosition().x, 0);
    if (paddle1.getPosition().y >= (GAME_HEIGHT - PADDLE_HEIGHT))
        paddle1.setPosition(paddle1.getPosition().x, GAME_HEIGHT - PADDLE_HEIGHT);
    if (paddle2.getPosition().y <= 0)
        paddle2.setPosition(paddle2.getPosition().x, 0);
    if (paddle2.getPosition().y >= (GAME_HEIGHT - PADDLE_HEIGHT))
        paddle2.setPosition(paddle2.getPosition().x, GAME_HEIGHT - PADDLE_HEIGHT);

    // Score points and reset positions
    if (ball.getPosition().x <= 0) {
        score.player2++;
        ball.setPosition(GAME_WIDTH / 2 - BALL_DIAMETER / 2, rand() % (GAME_HEIGHT - BALL_DIAMETER));
        ball.setRandomDirection();
        std::cout << "Player 2: " << score.player2 << std::endl;
    }
    if (ball.getPosition().x >= GAME_WIDTH - BALL_DIAMETER) {
        score.player1++;
        ball.setPosition(GAME_WIDTH / 2 - BALL_DIAMETER / 2, rand() % (GAME_HEIGHT - BALL_DIAMETER));
        ball.setRandomDirection();
        std::cout << "Player 1: " << score.player1 << std::endl;
    }
}

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode(GAME_WIDTH, GAME_HEIGHT), "Pong Game");
    window.setFramerateLimit(60);

    // Create paddles, ball, and score
    Paddle paddle1(0, GAME_HEIGHT / 2 - PADDLE_HEIGHT / 2, 1);
    Paddle paddle2(GAME_WIDTH - PADDLE_WIDTH, GAME_HEIGHT / 2 - PADDLE_HEIGHT / 2, 2);
    Ball ball(GAME_WIDTH / 2 - BALL_DIAMETER / 2, rand() % (GAME_HEIGHT - BALL_DIAMETER));
    Score score;

    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                paddle1.keyPress(event.key.code);
                paddle2.keyPress(event.key.code);
            }
            if (event.type == sf::Event::KeyReleased) {
                paddle1.keyRelease(event.key.code);
                paddle2.keyRelease(event.key.code);
            }
        }

        // Move paddles and ball
        paddle1.move();
        paddle2.move();
        ball.move();

        // Check collisions
        checkCollision(ball, paddle1, paddle2, score);

        // Draw everything
        window.clear();
        paddle1.setFillColor(sf::Color::Blue);
        paddle2.setFillColor(sf::Color::Red);
        ball.setFillColor(sf::Color::White);
        window.draw(paddle1);
        window.draw(paddle2);
        window.draw(ball);
        score.draw(window);
        window.display();
    }

    return 0;
}

