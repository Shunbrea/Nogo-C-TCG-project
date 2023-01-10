/**
 * Framework for NoGo and similar games (C++ 11)
 * agent.h: Define the behavior of variants of the player
 *
 * Author: Theory of Computer Games
 *         Computer Games and Intelligence (CGI) Lab, NYCU, Taiwan
 *         https://cgilab.nctu.edu.tw/
 */

#pragma once
#include <string>
#include <random>
#include <sstream>
#include <map>
#include <type_traits>
#include <algorithm>
#include <fstream>
#include <ctime>
#include "board.h"
#include "action.h"
// #include "mctstree.h"
#include "mcrave.h"
#include "pynet.h"

class agent {
public:
	agent(const std::string& args = "") {
		std::stringstream ss("name=unknown role=unknown " + args);
		for (std::string pair; ss >> pair; ) {
			std::string key = pair.substr(0, pair.find('='));
			std::string value = pair.substr(pair.find('=') + 1);
			meta[key] = { value };
		}
	}
	virtual ~agent() {}
	virtual void open_episode(const std::string& flag = "") {}
	virtual void close_episode(const std::string& flag = "") {}
	virtual action take_action(const board& b) { return action(); }
	virtual bool check_for_win(const board& b) { return false; }

public:
	virtual std::string property(const std::string& key) const { return meta.at(key); }
	virtual void notify(const std::string& msg) { meta[msg.substr(0, msg.find('='))] = { msg.substr(msg.find('=') + 1) }; }
	virtual std::string name() const { return property("name"); }
	virtual std::string role() const { return property("role"); }

protected:
	typedef std::string key;
	struct value {
		std::string value;
		operator std::string() const { return value; }
		template<typename numeric, typename = typename std::enable_if<std::is_arithmetic<numeric>::value, numeric>::type>
		operator numeric() const { return numeric(std::stod(value)); }
	};
	std::map<key, value> meta;
};

/**
 * base agent for agents with randomness
 */
class random_agent : public agent {
public:
	random_agent(const std::string& args = "") : agent(args) {
		if (meta.find("seed") != meta.end())
			engine.seed(int(meta["seed"]));
	}
	virtual ~random_agent() {}

protected:
	std::default_random_engine engine;
};

/**
 * player for both side
 * write by eric lin
 */
class player : public agent {
public:
	enum algorithms { random = 0u, mcts = 1u, alphabeta = 2u, rave = 3u, zero = 4u};
	virtual std::string search() const { return property("search"); }

	player(const std::string& args = "") : agent("role=unknown " + args),
		space(board::size_x * board::size_y), who(board::empty) {
		if (name().find_first_of("[]():; ") != std::string::npos)
			throw std::invalid_argument("invalid name: " + name());
		if (role() == "black") who = board::black;
		if (role() == "white") who = board::white;
		if (who == board::empty)
			throw std::invalid_argument("invalid role: " + role());
		
		// construct player with algorithms
		if (meta.find("search") == meta.end()){

			// player with random algorithms
			algo = algorithms::random;
			cerr << "player " << role() << " : use random" << endl;

			// parameter setting
			if (meta.find("seed") != meta.end())
				engine.seed(int(meta["seed"]));
			for (size_t i = 0; i < space.size(); i++)
				space[i] = action::place(i, who);

		} else if (search() == "MCTS") {

			// player with MCTS algorithms
			algo = algorithms::mcts;
			cerr << "player " << role() << " : use " << search() << endl;

			// parameter setting
			if (meta.find("seed") != meta.end())
				MCtree.setseed(int(meta["seed"]));

		} else if (search() == "alpha-beta") {
			// player with alpha-beta algorithms
			algo = algorithms::alphabeta;
		} else if (search() == "mc-rave") {
			// player with MC-rave algorithms
			algo = algorithms::rave;
			cerr << "player " << role() << " : use " << search() << endl;

			// parameter setting
			if (meta.find("seed") != meta.end())
				MCravetree.setseed(int(meta["seed"]));
			if (meta.find("nodeinit") != meta.end())
			    MCravetree.set_nodeinit(true);
		} else if (search() == "zero") {
			// player with alphazero algorithms
			algo = algorithms::zero;
			cerr << "player " << role() << " : use " << search() << endl;

			// parameter setting
			
		} else {
			// TODO
		}
	}

	virtual void open_episode(const std::string& flag = "") {
		if (algo == algorithms::zero){
			pynet = new PyNNet;
		}
	}
	virtual void close_episode(const std::string& flag = "") {
		if (algo == algorithms::zero){
			delete pynet;
		}
	}

	virtual action take_action(const board& state) {
		switch (algo) {
			case (mcts):
				MCtree.deleteNodes();
				// cerr << "delete" << endl;
				MCtree.setroot(state);
				// cerr << "set" << endl;
				return MCtree.uctsearch(std::stoi(property("timeout")));
			case (random):
				std::shuffle(space.begin(), space.end(), engine);
				for (const action::place& move : space) {
					board after = state;
					if (move.apply(after) == board::legal)
						return move;
				}
				return action();
			case (rave):
				MCravetree.deleteNodes();
				// cerr << "delete" << endl;
				MCravetree.setroot(state);
				// cerr << "set" << endl;
				return MCravetree.mcravesearch(std::stoi(property("timeout")));
			case (zero):
			{
				size_t act_ind = pynet->GetAction(state, who);
				action::place pymove(act_ind, who);
				return pymove;
			}
			case (alphabeta):
				return action();
			default:
				return action();
		}
	}

private:
	std::vector<action::place> space;
	board::piece_type who;
	algorithms algo;
	MctsTree MCtree;
	McraveTree MCravetree;
	PyNNet* pynet=0;

protected:
	std::default_random_engine engine;
};
