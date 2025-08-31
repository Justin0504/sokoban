#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // 创建窗口，大小为 800x600，标题为 "SFML Test"
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Test");

    // 检查是否成功创建窗口
    if (!window.isOpen()) {
        std::cerr << "Failed to create SFML window!" << std::endl;
        return -1;
    }

    // 创建一个圆形
    sf::CircleShape circle(50); // 半径为 50 像素
    circle.setFillColor(sf::Color::Green); // 设置圆形为绿色
    circle.setPosition(375, 275); // 圆心放在窗口中心

    // 主循环
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // 如果用户关闭窗口
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 清除窗口内容并设置背景为黑色
        window.clear(sf::Color::Black);

        // 绘制圆形
        window.draw(circle);

        // 显示内容
        window.display();
    }

    return 0;
}