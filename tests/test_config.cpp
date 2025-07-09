#define CATCH_CONFIG_MAIN
#include "../libs/catch.hpp"

extern "C" {
  #include "../config.h"
}

TEST_CASE("Receiver mode") {
  char *argv[] = {
    (char*)"./main",
    (char*)"receiver"
  };

  const char *arg = argv[1]; 
  Mode mode = parse_mode(arg);
  REQUIRE(mode == RECEIVER);
}

TEST_CASE("Nullptr cfg destroy") {
  Config *cfg = NULL;
  int n = destroy_config(cfg);
  REQUIRE(n < 0);
}

TEST_CASE("Parse valid sender config") {
  char *argv[] = {
    (char*)"./main",
    (char*)"--mode=sender",
    (char*)"--port=1234",
    (char*)"--addr=127.0.0.1",
    (char*)"--path=/tmp/file"
  };
  int argc = sizeof(argv) / sizeof(argv[0]);

  Config *cfg = parse_config(argc, argv);
  REQUIRE(cfg != nullptr);
  REQUIRE(cfg->mode == SENDER);
  REQUIRE(cfg->port == 1234);
  REQUIRE(std::string(cfg->addr) == "127.0.0.1");
  REQUIRE(std::string(cfg->path) == "/tmp/file");

  int n = destroy_config(cfg);
  REQUIRE(n == 0);
}

TEST_CASE("Invalid port") {
  char *argv[] = {
    (char*)"./main",
    (char*)"--mode=sender",
    (char*)"--port=-1",
    (char*)"--addr=127.0.0.1",
    (char*)"--path=/tmp/file"
  };
  int argc = sizeof(argv) / sizeof(argv[0]);

  Config *cfg = parse_config(argc, argv);
  REQUIRE(cfg == nullptr); 
}

TEST_CASE("Invalid flag") {
  char *argv[] = {
    (char*)"./main",
    (char*)"--invalid=sender",
  };
  int argc = sizeof(argv) / sizeof(argv[0]);

  Config *cfg = parse_config(argc, argv);
  REQUIRE(cfg == nullptr); 
}

TEST_CASE("Invalid mode fails") {
  char *argv[] = {
    (char*)"./main", (char*)"--mode=invalid", (char*)"--port=1234"
  };
  Config *cfg = parse_config(3, argv);
  REQUIRE(cfg == nullptr);
}

