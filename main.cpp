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
#include "state-spaces/klotski_boards.hpp"


#include "GMMP/GraphMM.hpp"
#include "GMMP/MM_3D.hpp"
#include "GMMP/MM_SS.hpp"

#include "StateSpaceGrapher/graph_support.hpp"

#include "core/graph.hpp"
#include "core/Astar.hpp"
#include "core/bfs.hpp"


tgui::ChildWindow::Ptr createTemplateWindow(tgui::Gui& gui,
                                            int sfmlWidth, int sfmlHeight,
                                            int ROWS, int COLUMNS,
                                            tgui::Color windowColor = tgui::Color(60, 60, 60)) {
    // Area cannot exceed 1/5th the area of the SFML 
    // Aspect ratio fits the number of board rows + columns
    const float maxArea = sfmlWidth * sfmlHeight / 5.f;
    int cell = static_cast<int>(std::floor(std::min({
        static_cast<float>(sfmlWidth)  / COLUMNS,
        static_cast<float>(sfmlHeight) / ROWS,
        std::sqrt(maxArea / (ROWS * COLUMNS))
    })));

    auto win = tgui::ChildWindow::create("Template");
    win->getRenderer()->setBackgroundColor(windowColor);

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

    // Cell name formatting is "r-c"
    // r in [0, ROWS); c in [0, COLUMNS)
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
        

    // ------- CHOOSING KLOTSKI INITIAL BOARD ------- //

    auto [boards, board_representations] = create_board_list();

    for (int i = 0; i < board_representations.size(); i++) {
        std::cout << "---------------------------------------\n";
        std::cout << "BOARD # " << i << ":" << std::endl;
        std::cout << board_representations[i];
    }

    std::cout << "CHOOSE YOUR BOARD (enter index of board to test)\n";
    int board_id;
    std::cin >> board_id;

    KlotskiBoard klotski_board = boards[board_id];


    // ------- TGUI WINDOW ------- //

    tgui::Gui gui(window);
    auto templateWindow = createTemplateWindow(gui, WIDTH, HEIGHT, klotski_board.width, klotski_board.height);


    // ------- GENERATING STATE SPACE ------- //
    std::vector<std::string> states;
    StateSpace klotski_ss = generate_klotski_ss(klotski_board, states);
    std::cout << "Num_nodes: " << klotski_ss.NUM_NODES << std::endl;
    std::cout << "Num_edges: " << klotski_ss.edges.size() << std::endl;

   int start_index = 0;
    int end_index = klotski_ss.NUM_NODES/2;
    std::cout << "Computing shortest path from\n";
    std::cout << states[start_index] << std::endl;
    std::cout << "to\n";
    std::cout << states[end_index] << std::endl;


        // ------- PERFORMING GRAPH SEARCH ------- //
    Graph klotski_graph = ss_to_graph(klotski_ss);
    int bfs_count = 0;
    int astar_count = 0;
    std::vector<int> bfs_path = bfs(klotski_graph, start_index, end_index, &bfs_count);
    std::vector<int> astar_path = a_star(klotski_graph, start_index, end_index, [](int, int) { return 0; }, &astar_count);
    
    std::vector<int>& optimal_path = bfs_path;

    std::cout << "Length of bfs path: " << optimal_path.size() << std::endl;
    std::cout << "Length of a* path: " << optimal_path.size() << std::endl;
    std::cout << std::endl;
    std::cout << "Number of nodes checked for BFS: " << bfs_count << std::endl;
    std::cout << "Number of nodes checked for A*: " << astar_count << std::endl;
    std::cout << std::endl;



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