#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/spdlog.h>
#include <map>
#include "initializations.hpp"

#ifndef TETRAGON_NO_LOG_EMOJIS
#	define TETRAGON_LOG_PATTERN "[%H:%M:%S.%e] (t%t) %^%E %G%$ : %v"
#else
#	define TETRAGON_LOG_PATTERN "[%H:%M:%S.%e] (t%t) %^%G%$ : %v"
#endif

class LogLevelEmojiFlag final : public spdlog::custom_flag_formatter {
	static const std::map<spdlog::level::level_enum, std::string> emojis;
public:

	void format(spdlog::details::log_msg const& msg, std::tm const&, spdlog::memory_buf_t& dest) override {
		const std::string& emoji = emojis.at(msg.level);
		dest.append(emoji.data(), emoji.data() + emoji.size());
	}

	std::unique_ptr<spdlog::custom_flag_formatter> clone() const override {
		return spdlog::details::make_unique<LogLevelEmojiFlag>();
	}
};

const std::map<spdlog::level::level_enum, std::string> LogLevelEmojiFlag::emojis = {
	{ spdlog::level::info,  "\uf05a" },     // Info circle
	{ spdlog::level::warn,  "\uf071" },     // Warning triangle 
	{ spdlog::level::err,   "\U000f068c" }, // Skull
	{ spdlog::level::debug, "\U000f00e4" }  // Bug
};

class ShortLevelFlag final : public spdlog::custom_flag_formatter {
	static const std::map<spdlog::level::level_enum, std::string> levelNames;
public:
	void format(spdlog::details::log_msg const& msg, std::tm const&, spdlog::memory_buf_t& dest) override {
		const std::string& levelName = levelNames.at(msg.level);
		dest.append(levelName.data(), levelName.data() + levelName.size());
	}

	std::unique_ptr<spdlog::custom_flag_formatter> clone() const override {
		return spdlog::details::make_unique<ShortLevelFlag>();
	}
};

const std::map<spdlog::level::level_enum, std::string> ShortLevelFlag::levelNames = {
	{ spdlog::level::info,  "INFO " },
	{ spdlog::level::warn,  "WARN " },
	{ spdlog::level::err,   "ERROR" },
	{ spdlog::level::debug, "DEBUG" }
};

void tetragon::init_logs() {
	auto formatter = std::make_unique<spdlog::pattern_formatter>();
	formatter->add_flag<LogLevelEmojiFlag>('E')
		.add_flag<ShortLevelFlag>('G')
		.set_pattern(TETRAGON_LOG_PATTERN);
	spdlog::set_formatter(std::move(formatter));
	spdlog::set_level(spdlog::level::debug);
}


void tetragon::init_glfw() {
	static bool initialized = false;
	if (initialized) return;
	glfwInit();
   	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	initialized = true;
}

void tetragon::init_glad() {
	static bool initialized = false;
	if (initialized) return;
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		spdlog::error("Failed to initialize GLAD, exiting the program");
		glfwTerminate();
		std::exit(-1);
	}
	initialized = true;
}
