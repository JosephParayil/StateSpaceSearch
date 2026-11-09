#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>


#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "sfml-util/sfml_util.hpp"
#include "sfml-3d/3d_camera.hpp"
#include "sfml-3d/3d_engine.hpp"
#include "sfml-3d/math4.hpp"
#include "state-spaces/ss.hpp"
#include "state-spaces/klotski.hpp"
#include "state-spaces/ss.hpp"

#include "GMMP/GraphMM.hpp"
#include "GMMP/MM_3D.hpp"
#include "GMMP/MM_SS.hpp"

#include "StateSpaceGrapher/graph_support.hpp"



tgui::ChildWindow::Ptr createTemplateWindow(tgui::Gui& gui,
                                            int sfmlWidth, int sfmlHeight,
                                            int ROWS, int COLUMNS,
                                            tgui::Color windowColor = tgui::Color(60, 60, 60)) {
    // Largest square cell size (in pixels) that satisfies all three limits.
    // Cells are square, so the window's aspect ratio is COLUMNS:ROWS.
    const float maxArea = sfmlWidth * sfmlHeight / 5.f;
    int cell = static_cast<int>(std::floor(std::min({
        static_cast<float>(sfmlWidth)  / COLUMNS,
        static_cast<float>(sfmlHeight) / ROWS,
        std::sqrt(maxArea / (ROWS * COLUMNS))
    })));

    auto win = tgui::ChildWindow::create("Template");
    win->getRenderer()->setBackgroundColor(windowColor);

    // The title bar and borders add to the total size, so shrink the cells
    // until the full window (getSize) respects the limits too.
    for (; cell > 0; --cell)
    {
        win->setClientSize({static_cast<float>(COLUMNS * cell),
                            static_cast<float>(ROWS * cell)});
        const tgui::Vector2f s = win->getSize();
        if (s.x <= sfmlWidth && s.y <= sfmlHeight && s.x * s.y <= maxArea)
            break;
    }

    win->setPosition({"(&.width - width) / 2", "(&.height - height) / 2"}); // centered
    gui.add(win, "template");

    // Cells: named "r-c", r in [0, ROWS), c in [0, COLUMNS)
    for (int r = 0; r < ROWS; ++r)
    {
        for (int c = 0; c < COLUMNS; ++c)
        {
            auto panel = tgui::Panel::create({static_cast<float>(cell), static_cast<float>(cell)});
            panel->setPosition({static_cast<float>(c * cell), static_cast<float>(r * cell)});
            panel->getRenderer()->setBackgroundColor(windowColor); // default = window colour
            win->add(panel, std::to_string(r) + "-" + std::to_string(c));
        }
    }
    return win;
}

void configureStateWindow_klotski(tgui::ChildWindow& window, std::string state)
{
    const tgui::Color background = window.getRenderer()->getBackgroundColor();

    std::istringstream rows(state);
    std::string rowStr;
    int r = 0;

    while (std::getline(rows, rowStr, '\n'))
    {
        std::istringstream cells(rowStr);
        std::string token;
        int c = 0;

        while (std::getline(cells, token, '\t'))
        {
            auto panel = window.get<tgui::Panel>(std::to_string(r) + "-" + std::to_string(c));
            if (panel)
            {
                tgui::Color fill = background;   // '_' (or anything unparseable) -> window colour

                if (!token.empty() && token != "_"
                    && token.find_first_not_of("0123456789") == std::string::npos)
                {
                    const auto rgb = color(std::stoull(token));
                    fill = tgui::Color(rgb[0], rgb[1], rgb[2]);
                }

                panel->getRenderer()->setBackgroundColor(fill);
            }
            ++c;
        }
        ++r;
    }
}



int main() {
    // ------- SFML + 3D CAMERA ------- //
    const int HEIGHT = 1400;
    const int WIDTH = 2100;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "State Space Grapher");
    Camera camera(window, 60.0f, 0.001f, 2.f, 10.f, 0.5f); 
        

    // ------- KLOTSKI INITIAL BOARD ------- //

KlotskiBoard klotski_board(7,7);

KlotskiRect rect0(3,0,1,2, KlotskiRect::MovementMode::Vertical);
klotski_board.addRect(rect0);
KlotskiRect rect1(0,3,2,1, KlotskiRect::MovementMode::Horizontal);
klotski_board.addRect(rect1);
KlotskiRect rect2(5,4,2,1, KlotskiRect::MovementMode::Horizontal);
klotski_board.addRect(rect2);


// ------- TGUI WINDOW ------- //

    tgui::Gui gui(window);
    auto templateWindow = createTemplateWindow(gui, WIDTH, HEIGHT, klotski_board.width, klotski_board.height);


    // ------- GENERATING STATE SPACE ------- //
    std::vector<std::string> states;
    StateSpace klotski_ss = generate_klotski_ss(klotski_board, states);
    std::cout << "Num_edges: " << klotski_ss.edges.size() << std::endl;
    
    std::vector<int> print_states = {0, 174};
    for (int i = 0; i < states.size(); i++) {
        for (int j : print_states) {
            if (i==j) {
                std::cout << states[i] << std::endl;
                break;
            }
        }
    }

    // ------- PERFORMING GRAPH SEARCH ------- //
    Graph klotski_graph = ss_to_graph(klotski_ss);
    std::vector<int> optimal_path = dijkstra_shortest_path(klotski_graph, 0, 174);
    std::cout << "Length of optimal path: " << optimal_path.size() << std::endl;
    


    // ------- CREATING 3D REPRESNTATION ------- //
    MM_SS my3DMM(states, klotski_ss.edges, gui, *templateWindow, configureStateWindow_klotski, optimal_path);
    std::cout << "mm created" << std::endl;
    std::cout << "number of states: " << states.size() << std::endl;


    bool IN_GUI = false;
    bool WAS_IN_GUI = false;
    while (window.isOpen()) {
        my3DMM.updateSelectionOnPoint(window, camera, sf::Vector2f(sf::Mouse::getPosition(window)));
        while (const auto& event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else {
                std::string handler_code = my3DMM.handleEvent(window, event, camera);
                IN_GUI = handler_code == "IN_GUI";
                //if (!WAS_IN_GUI) 
                camera.handleEvent(event);
            }   
        } 
        // - - UPDATES - - 
        if (true) {//(!WAS_IN_GUI) {
            camera.update();
        }
        WAS_IN_GUI = IN_GUI;
        
        my3DMM.computePhysics();

        // - - DRAWING - -
        window.clear();
        my3DMM.render(window, camera);
        camera.drawCrosshairIfNeeded(window);
        window.display();
    }

    return 0;
}