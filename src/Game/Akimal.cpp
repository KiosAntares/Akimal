#include "Akimal.h"

Akimal::Akimal(string _path)
{
	// values are assigned by default
	Load(_path);	// path is updated here
}

Akimal::~Akimal()
{
	Clear();
}

void Akimal::Dispose(node_p n)
{
	if (n != nullptr)
	{
		Dispose(n->left);
		Dispose(n->right);
		delete n;
	}
}

uint Akimal::getAnswerNum(node_p n) const
{
	if (n == nullptr)
		return 0;

	if (n->isLeaf())
		return 1;

	return getAnswerNum(n->left) + getAnswerNum(n->right);
}

void Akimal::Game(node_p current)
{
	string input;

	if (current->isParent())
	{
		std::cout<< current->key << "? (y/n):";
		do
		{
			std::cin >> input;
			if (PositiveAnswer(input))
				return Game(current->left);
			else if (NegativeAnswer(input))
				return Game(current->right);
			else
				std::cout<< "Not Valid\n";
		} while (!PositiveAnswer(input) && !NegativeAnswer(input));
	}
	else
	{
		std::cout<< "I guess " << current->key << ". (Yes/No):";
		do
		{
			std::cin >> input;
			if (PositiveAnswer(input))
				std::cout<< "I won.\n";
			else if (NegativeAnswer(input))
				AddEntry(current);
			else
				std::cout<< "Not Valid.\n";
		} while (!PositiveAnswer(input) && !NegativeAnswer(input));
	}
}

void Akimal::AddEntry(node_p current)
{
	string animal, question,
		tmp = current->key;
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
	std::cout<< "What animal did you mean?: ";
	getline(std::cin, animal);
	std::cout<< "What is the difference between the two?: ";
	getline(std::cin, question);

	question.back() == '?' ?
		question.pop_back 
		: void();

	current->key = question;
	current->left = new str_node(animal);
	current->right = new str_node(tmp);
}

void Akimal::Save(string _path)
{
	ofstream file(_path);

	// if tree is empty, there is nothing to save
	if (Empty())
		std::clog << "Akimal::Save(string) @ Cannot save data to " << _path << ": tree is empty." << std::endl;

	// ensure file can be overwritten
	else if (file.good())
		Save(file, root);

	else
		std::clog << "Akimal::Save(string) @ There was an error while trying to save current configuration in " << _path << "." << std::endl;

	file.close();
}

void Akimal::Save(ofstream& file, node_p node)
{
	if (node->isParent())		// a question
	{
		file << '\n' << QUESTION_ID << node->key;
		Save(file, node->left);
		Save(file, node->right);
	}

	else						// an answer
		file << '\n' << node->key;
}

void Akimal::Load(string _path)
{
	// clear previous state and configuration
	Clear();
	path = _path;

	ifstream file(path);

	// have to ensure file can be read 
	//	(not empty or damaged)

	// check if it is empty
	if (file.peek() == ifstream::traits_type::eof())
		std::clog << "Akimal::Load(string) @ File " << _path << " is empty! No data can be found." << std::endl;

	// prevent any reading error
	else if (file.good())
	{
		char dummy = char();
		// get number of lines during input process
		while (dummy != QUESTION_ID)
			file >> dummy;
		file.unget();

		int lines = Load(file, root);

		// check if eof wasn't reached
		if ((int) file.tellg() != ifstream::traits_type::eof())
		{
			std::clog << "Akimal::Load(string) @ It was impossible to read the entire file at " << _path << "." << std::endl;
			Clear();
		}

		// ensure no node has only one child
		else if (lines % 2 == 0)
		{
			std::clog << "Akimal::Load(string) @ An error occurred while reading file " << path << ": one or more lines are missing!" << std::endl;
			Clear();
		}

		// after all checks, we know configuration is safe
		else
			size = lines;
	}

	else
		std::clog << "Akimal::Load(string) @ File " << _path << " cannot be loaded." << std::endl;

	file.close();
}

uint Akimal::Load(ifstream& file, node_p& n)
{
	// if eof was reached, no new line
	if (file.eof())
		return 0;

	string line;

	if (getline(file, line))	// input successful
	{
		// check if line begins with '?' --> is a characteristic / question
		if (line.at(0) == QUESTION_ID)
		{
			n = new str_node(line.substr(1));
			return 1 + Load(file, n->left) + Load(file, n->right);
		}

		else
			n = new str_node(line);
	}

	// if either there was a reading error in the line,
	//	or it was a corrent answer line
	return 1;
}