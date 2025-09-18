#pragma once
#include "WelcomeWindow.hpp"
#include "MainWindow.hpp"
#include <NetLib.hpp>
#include <thread>
#include <queue>

class GiperbolaBookApp
{
public:
	GiperbolaBookApp(const std::string& ip, uint16_t port, uint16_t local_port);

public:
	void run();

private:
	MainWindow main_window_;
	std::shared_ptr<NetLib> net_;
	std::string ip_;
	uint16_t port_, local_port_;
	std::string nickname_;
};