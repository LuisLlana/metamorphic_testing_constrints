// Este programa calcula los fu correspondiente a M3 
// donde se incrementa el valor de una duration 
// En concreto ha de ser mayor que la suma de todas las durations

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <regex>
using namespace std;

struct tFiles {
	string nombre = "";
	string texto = "";
};


void procesa(string file_name, string fu_name);
void procesa_todos(string file_name, string fu_name);


int main() {

	string path_file = "rcpsp\\data_psplib\\j30\\J30";
	string path_fu = "rcpsp\\fu\\M3\\j30\\J30";
	procesa_todos(path_file, path_fu);

	path_file = "rcpsp\\data_psplib\\j60\\J60";
	path_fu = "rcpsp\\fu\\M3\\j60\\J60";
	procesa_todos(path_file, path_fu);

	path_file = "rcpsp\\data_psplib\\j90\\J90";
	path_fu = "rcpsp\\fu\\M3\\j90\\J90";
	procesa_todos(path_file, path_fu);

	path_file = "rcpsp\\data_psplib\\j120\\J120";
	path_fu = "rcpsp\\fu\\M3\\j120\\J120";
	procesa_todos(path_file, path_fu);

}

void procesa_todos(string path_file, string path_fu) {

	string s = "";

	for (int i = 1; i <= 48; i++) {  // 48
		for (int j = 1; j <= 10; j++) {  // 10
			string path_file_aux = path_file + "_" + to_string(i) + "_" + to_string(j);
			string path_fu_aux = path_fu + "_" + to_string(i) + "_" + to_string(j);
			procesa(path_file_aux, path_fu_aux);
		}
	}
}

void procesa(string file_name, string fu_name) {

	ifstream in(file_name + ".dzn");
	ofstream out(fu_name + "_fu_d.dzn");   // OJO M3

	std::string s = "";

	// Las líneas se leen y escriben directamente pq no cambian 
	// hasta encontrar una línea que comienza con d

	std::string prefix = "d";

	// Trabajo con expresiones regulares para quitar = ; [ ]
	std::regex word_regex("(\\w+)");

	int num_tasks = 0;
	std::getline(in, s);
	while (s.compare(0, prefix.size(), prefix) != 0) {

		// Necesito saber el número de tareas pero está pegado al ; final

		// Uso iteradores 
		std::sregex_iterator i = std::sregex_iterator(s.begin(), s.end(), word_regex);
		std::smatch match = *i;
		std::string match_str = match.str();

		if (match_str.compare("n_tasks") == 0) {
			++i;
			match = *i;
			match_str = match.str();
			num_tasks = stoi(match_str);  // casting de string a int
		}

		out << s << endl;
		std::getline(in, s);
	}

	// la línea que tengo en s es de la forma d = [...]

	int var_sum = 1;  // empieza en 1 pq es mayor estricto a la suma de todas las duraciones

	std::sregex_iterator i = std::sregex_iterator(s.begin(), s.end(), word_regex);
	std::smatch match = *i;
	std::string match_str = match.str();

	string* array_din_durations = new string[num_tasks];

	// Guardo los datos d = [ ... ]; en un array dinámico array_din_durations
	// y de paso voy haciendo la suma
	for (int k = 0; k < num_tasks; k++) {
		++i;
		match = *i;
		match_str = match.str();
		array_din_durations[k] = match_str;
		var_sum += stoi(match_str);
	}

	// Genero la salida

	out << "d = [ ";

	for (int k = 0; k < num_tasks - 1; k++) {
		if (k == 0) {
			out << to_string(var_sum);
		}
		else {
			out << array_din_durations[k];   // añado elemento a elemento
		}
		out << ", ";
	}

	// Escribo el último elemento y cierro
	out << array_din_durations[num_tasks-1] << " ];\n";

	// El resto de líneas se leen y escriben directamente pq no cambian

	getline(in, s);
	while (in) {
		out << s + "\n";
		getline(in, s);
	}

	out.close();
	in.close();
}