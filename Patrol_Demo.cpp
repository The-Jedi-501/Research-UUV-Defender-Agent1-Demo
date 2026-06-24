/*
Purpose of the code itself: Was to create our UUV defender agents as example programs 

The demo here would be to done in the following steps:
1) recreate the map itself and understand it to some level
2) be able to select point and how that process would work alongside how the safety check of "hey theres land between A and B cant do it"
3) moving the demo uuv from A -> B -> A ->..........

Future Ideas to add (Main goal rn is to just get the idea working code of example and then move unto next as EVERYTHING DOESNT NEED TO BE DONE BY FRI meet):
1) Can i say how long/repeats of the loop to do ie do A -> B only 5 times then stay at A
2) Can i create a vector in whcih i just keep appedning points so i can do A -> B -> C ...... Essentially would be the prexisiting logic just a giant while loop
3) Could I have it be Hey do you want UUV static like orginal Code or Dynamic that I made 
4) Between all points ask do you want to do another point (If user.input.dynamic == a then let them add another point else stop adding points) 
5) Could I use physics here and say hey I want to travel from point A to B which has a distance of X and get there in 30 sec (Distance / time = Speed) adjust in real time per point speed essentially 

Terminal stuff you might need to know cause I couldnt remember:
cmake --build build             This was used to esstnally make and fix the map at one point
./build/PatrolDemo              This actully ran the code but would take a min or two to actully load it 

not code based the the include SFML no idea why its red squigles are included but it works 
*/

#include "mapCreation.h"
#include <SFML/Graphics.hpp> //LITERALLY NO IDEA WHY ITS RED ITS DUMB BUT THE PROGRAM STILL RUNS AHHHHHHH
#include <iostream>
#include <cmath>

int main() {
    std::cout << "PROGRAM STARTED\n"; //this was to make sure it was runnign properly since i need to do dumb stuff in terminal ie ./build/PatrolDemo
    MapCreation map("Maps/pearlHarbour/Harbour_Depth_Area.shp", 100);

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(700, 740)),
        "Patrol Defender Demo"
    );
    window.setFramerateLimit(30);       //COULD IT RUN AT 144FPS FINNA MAKE IT RUN BUTTERY SMOOTH 

    int n = map.getCellsN();            // how many cells the grid has (100x100)
    float cellSize = 700.0f / n;        // how big each cell is in pixels (7px)
    const auto& grid = map.getGrid();   // the actual grid — 0 = water, 1 = land

    // patrol points
    int pointARow = -1, pointACol = -1; // where user clicked for A
    int pointBRow = -1, pointBCol = -1; // where user clicked for B
    bool waitingForA = true;            // are we still waiting for first click?
    bool readyToPatrol = false;         // do we have both points yet?

    // defender current position (starts at A)
    float defRow = -1, defCol = -1;     // defender's current position

    // movement direction — true = going toward B, false = going back to A
    bool goingToB = true;               // which direction is it heading?

    // movement speed — how many pixels per frame --- greater = faster ie .2f faster vs .5f wicked fast
    float speed = 0.08f;

    // status message
    std::string status = "Click a water cell to set Point A";
    std::cout << status << "\n";

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    int col = static_cast<int>(click->position.x / cellSize);
                    int row = static_cast<int>(click->position.y / cellSize);

                    // bounds check
                    if (row < 0 || row >= n || col < 0 || col >= n) continue;

                    if (grid[row][col] == 0) {  // water only --- is it water?
                        if (waitingForA) {      // first click = point A
                            pointARow = row;
                            pointACol = col;
                            defRow = row;
                            defCol = col;
                            waitingForA = false;
                            status = "Point A set! Now click Point B";
                            std::cout << status << "\n";
                        } else if (pointBRow == -1) {   // second click = point B
                    if (row == pointARow && col == pointACol) {
                        std::cout << "Point B can't be same as Point A\n";
                    } else {
                        // safety check — walk from A to B and make sure no land in the way
                        bool pathClear = true;
                        int steps = 50;
                        for (int i = 0; i <= steps; i++) {
                            float t = (float)i / steps;
                            int checkRow = pointARow + (row - pointARow) * t;
                            int checkCol = pointACol + (col - pointACol) * t;
                            if (grid[checkRow][checkCol] == 1) {
                                pathClear = false;
                                std::cout << "Land in the way — pick a different Point B\n";
                                break;
                            }
                        }
                        if (pathClear) {
                            pointBRow = row;
                            pointBCol = col;
                            readyToPatrol = true;
                            std::cout << "Path clear — patrolling!\n";
                        }
                    }
                    }
                    } else {    // error if they click land
                        std::cout << "That's land — pick a water cell\n";
                    }
                }
            }
        }

        // movement logic
        if (readyToPatrol) {
            float targetRow = goingToB ? pointBRow : pointARow; // where are we heading?
            float targetCol = goingToB ? pointBCol : pointACol;

            float dr = targetRow - defRow;                      // how far away in rows
            float dc = targetCol - defCol;                      // how far away in cols
            float dist = std::sqrt(dr * dr + dc * dc);          // actual distance

            if (dist < speed) {                                 // close enough — we arrived, flip direction
                // reached the target point — switch direction
                defRow = targetRow;
                defCol = targetCol;
                goingToB = !goingToB;
                std::cout << (goingToB ? "Heading to B\n" : "Heading back to A\n");
            } else {                                            // move one step toward target
                // move one step toward target
                defRow += (dr / dist) * speed;
                defCol += (dc / dist) * speed;
            }
        }

        // draw
        window.clear(sf::Color::Black);

        // draw map grid
        sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
        for (int row = 0; row < n; row++) {
            for (int col = 0; col < n; col++) {
                cell.setPosition(sf::Vector2f(col * cellSize, row * cellSize));
                if (grid[row][col] == 0)
                    cell.setFillColor(sf::Color(20, 50, 120));   // water
                else
                    cell.setFillColor(sf::Color(50, 110, 50));   // land
                window.draw(cell);
            }
        }

        // draw point A — green circle
        if (pointARow != -1) {
            sf::CircleShape a(cellSize * 0.4f);
            a.setFillColor(sf::Color(0, 200, 100));
            a.setOrigin(sf::Vector2f(cellSize * 0.4f, cellSize * 0.4f));
            a.setPosition(sf::Vector2f((pointACol + 0.5f) * cellSize, (pointARow + 0.5f) * cellSize));
            window.draw(a);
        }

        // draw point B — red circle
        if (pointBRow != -1) {
            sf::CircleShape b(cellSize * 0.4f);
            b.setFillColor(sf::Color(220, 50, 50));
            b.setOrigin(sf::Vector2f(cellSize * 0.4f, cellSize * 0.4f));
            b.setPosition(sf::Vector2f((pointBCol + 0.5f) * cellSize, (pointBRow + 0.5f) * cellSize));
            window.draw(b);
        }

        // draw defender — cyan diamond
        if (readyToPatrol || pointARow != -1) {
            float cx = (defCol + 0.5f) * cellSize;
            float cy = (defRow + 0.5f) * cellSize;
            float s = cellSize * 0.45f;
            sf::RectangleShape def(sf::Vector2f(s, s));
            def.setOrigin(sf::Vector2f(s / 2.f, s / 2.f));
            def.setPosition(sf::Vector2f(cx, cy));
            def.setRotation(sf::degrees(45.f));
            def.setFillColor(sf::Color(0, 210, 255));
            def.setOutlineColor(sf::Color::White);
            def.setOutlineThickness(1.5f);
            window.draw(def);
        }


        // 1. draw every cell blue or green based on grid value
        // 2. draw a green circle at point A
        // 3. draw a red circle at point B  
        // 4. draw a cyan rotating square at the defender's current position
        window.display();   // push it all to screen
    }

    return 0;
}