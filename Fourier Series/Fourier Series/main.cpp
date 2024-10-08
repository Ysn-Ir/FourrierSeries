#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

#define PI 3.14159265358979323846
const int w = 1200, l = 600;
const float posx = 200.f, posy = 200.f;

// Vector class contains frequency, amplitude, and phase
class vectro {
public:
    sf::Vector2f vec;
    float amp, phase;
    int freq;
};
// Function declaration

std::vector<vectro> dft(const std::vector<float>& values);
std::vector<float> generateTriangleWave(int samples, int periods, float amplitude);
std::vector<sf::Vector2f> generateCircleSignal(int samples, float amplitude);
std::vector<sf::Vector2f> generateStarSignal(int numPoints, float innerRadius, float outerRadius);
std::vector<sf::Vector2f> imageToSignal(const sf::Image& image);
sf::Vector2f eclypse(std::vector<sf::CircleShape>& circlesX, float x, float y,float& clk,float rotation, std::vector<vectro>& Fourrier);
std::vector<sf::Vector2f> image_loader(std::string path);
void handleViewAdjustment(sf::RenderWindow& window, sf::View& view, const sf::Event& event);

    int main() {

    // Defining constants
    float clk = 0;

    // Setting the signal
         //int samples = 100; // number of samples
         //float amplitude = 100.0f; // amplitude of the wave
         //std::vector<sf::Vector2f> Signal = generateCircleSignal(samples, amplitude);
    //star
    int numPoints = 10;  // Number of points in the star
    float innerRadius = 30.0f;  // Inner radius of the star
    float outerRadius = 100.0f;  // Outer radius of the star

    // image source // files path

    std::string imagePath = "homer.png";

    std::vector<sf::Vector2f> Signal = image_loader(imagePath);

    std::vector<float> SignalX, SignalY;

    for (int i = 0; i < Signal.size(); i+=5) {
        SignalX.push_back(Signal[i].x);
        SignalY.push_back(Signal[i].y);
    }

    std::vector<vectro> FourrierY = dft(SignalY);
    std::vector<vectro> FourrierX = dft(SignalX);

    int size = FourrierY.size();

    // Rendering window
    sf::RenderWindow window(sf::VideoMode(w, l), "Fourier Series", sf::Style::Default);
    window.setFramerateLimit(60);
    std::vector<sf::CircleShape> circlesX(size);
    std::vector<sf::CircleShape> circlesY(size);

    // Initializing circles
    for (int i = 0; i < size; i++) {
        sf::CircleShape cir(2 * FourrierY[i].amp);
        cir.setOutlineThickness(2.f);
        cir.setFillColor(sf::Color::Black);
        cir.setPosition(sf::Vector2f(posx, posy));
        circlesY[i] = cir;
    }
    for (int i = 0; i < size; i++) {
        sf::CircleShape cir(2 * FourrierX[i].amp);
        cir.setOutlineThickness(2.f);
        cir.setFillColor(sf::Color::Black);
        cir.setFillColor(sf::Color::Transparent);
        cir.setPosition(sf::Vector2f(posx, posy));
        circlesX[i] = cir;
    }

    std::vector<sf::Vertex> line;
    sf::Vertex lpos[2];
    //positions? 
    // 
    // Main loop

    sf::View v;
    v.setSize(2800, 1200);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }         
            handleViewAdjustment(window, v, event);

        }

        // Adjust view based on input

        window.setView(v);

        // The main function

        float x = eclypse(circlesX, 900, 100, clk, 0, FourrierX).x;
        float y = eclypse(circlesY, 100, 400, clk, PI/2, FourrierY).y;

        //moving graph
      /*  for (int i = 0; i < line.size(); i++)
            line[i].position.x -= clk ;*/

      /*  if (line.size() > 1000) {
            line.erase(line.begin());
        }*/

        // Rendering and setting lines

        window.clear();
        line.push_back(sf::Vertex(sf::Vector2f(x, y), sf::Color::White));

        // Handle and draw circles vector

        for (int i = 0; i < size; ++i) {
            window.draw(circlesX[i]);
            window.draw(circlesY[i]);
        }
      // Draw the function

        if (line.size() > 1) {
            window.draw(&line[0], line.size(), sf::LinesStrip);
        }
        // Display everything

        window.display();
        window.setView(v);

        // Update the clock

        const float  dt = 2 * PI / size;
        clk -= dt;
    }

    return 0;
}

// Signal treating

std::vector<vectro> dft(const std::vector<float>& values) {

    std::vector<vectro> X;
    const int N = values.size();
    for (int i = 0; i < N; i++) {
        float re = 0, im = 0;
        for (int j = 0; j < N; ++j) {
            const float phi = (2 * PI * i * j) / N;
            re += values[j] * cos(phi);
            im -= values[j] * sin(phi);
        }
        re = re / N;
        im = im / N;
        sf::Vector2f vecX = sf::Vector2f(re, im);
        int freq = i;
        float amp = sqrtf(re * re + im * im);
        float phase = atan2(im, re);
        vectro Xv = { vecX, amp, phase, freq };
        //std::cout << "Freq: " << freq << " Amp: " << amp << " Phase: " << phase << " Re: " << re << " Im: " << im << "\n";
        X.push_back(Xv);

    }

    return X;
}


//the eclpse function

sf::Vector2f eclypse(std::vector<sf::CircleShape> &circlesX, float x , float y,float &clk,float rotation ,std::vector<vectro> &Fourrier) {
    
    for (int i = 0; i < Fourrier.size(); i++) {
        float prevx = x;
        float prevy = y;
        int n = Fourrier[i].freq;
        float phase = Fourrier[i].phase;
        x += Fourrier[i].amp * cos(clk * n + phase + rotation);
        y += Fourrier[i].amp * sin(clk * n + phase + rotation);

        circlesX[i].setPosition(prevx - circlesX[i].getRadius(), prevy - circlesX[i].getRadius());
    }
    return sf::Vector2f(x, y);
}

// Generate a triangle wave signal

std::vector<float> generateTriangleWave(int samples, int periods, float amplitude) {
    std::vector<float> signal(samples);
    for (int i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / samples * periods;
        signal[i] = amplitude * (2 * std::abs(2 * (t - std::floor(t + 0.5))) - 1);
    }
    return signal;
}

// Generate a circle signal

std::vector<sf::Vector2f> generateCircleSignal(int samples, float amplitude) {
    std::vector<sf::Vector2f> signal(samples);
    for (int i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / samples * 2 * PI;
        signal[i] = sf::Vector2f(amplitude * cos(t), amplitude * sin(t));
    }
    return signal;
}

//Generate a star signal

std::vector<sf::Vector2f> generateStarSignal(int numPoints, float innerRadius, float outerRadius) {
    std::vector<sf::Vector2f> signal(numPoints);
    float angleIncrement = 2 * PI / numPoints;

    for (int i = 0; i < numPoints; ++i) {
        float angle = i * angleIncrement;

        // Alternate between inner and outer radius

        float radius = (i % 2 == 0) ? outerRadius : innerRadius;

        float x = radius * cos(angle);
        float y = radius * sin(angle);

        signal[i] = sf::Vector2f(x, y);
    }

    return signal;
}

//image to signal

std::vector<sf::Vector2f> imageToSignal(const sf::Image& image) {

    std::vector<sf::Vector2f> signal;

    // Get image dimensions
    unsigned int width = image.getSize().x;
    unsigned int height = image.getSize().y;

    // Iterate through each pixel
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            sf::Color pixel = image.getPixel(x, y);

            // Check if the pixel is black (or close to black)
            if (pixel.r < 50 && pixel.g < 50 && pixel.b < 50) {
                // Add the black pixel's coordinates to the signal
                signal.push_back(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
            }
        }
    }

    return signal;
}

    std::vector<sf::Vector2f> image_loader(std::string path) {

        sf::Image image;

        if (!image.loadFromFile(path)) {
           std::cout << "Failed to load image!" << std::endl;
           return std::vector<sf::Vector2f>();
        }
    
        // Convert the image to a signal

         return imageToSignal(image);
    }
  // view   
    void handleViewAdjustment(sf::RenderWindow& window, sf::View& view, const sf::Event& event) {

        static sf::Vector2f previousMousePosition;
        static bool isDragging = false;

        // Mouse input for dragging
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2f currentMousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (!isDragging) {
                isDragging = true;
                previousMousePosition = currentMousePosition;
            }
            else {
                sf::Vector2f delta = previousMousePosition - currentMousePosition;
                view.move(delta);
                previousMousePosition = currentMousePosition;
            }
        }
        else {
            isDragging = false;
        }

        // Mouse wheel for zooming
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.delta > 0) {
                view.zoom(0.9f);  // Zoom in
            }
            else if (event.mouseWheelScroll.delta < 0) {
                view.zoom(1.1f);  // Zoom out
            }
        }

        // Keyboard input for panning
        const float moveSpeed = 10.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            view.move(0, -moveSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            view.move(0, moveSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            view.move(-moveSpeed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            view.move(moveSpeed, 0);
        }

        // Keyboard input for zooming

        const float zoomFactor = 0.9f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            view.zoom(zoomFactor);  // Zoom in
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            view.zoom(1.0f / zoomFactor);  // Zoom out
        }

    }
