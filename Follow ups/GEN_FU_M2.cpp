// Este programa calcula los fu correspondiente a M2 
// donde se CREA UN CICLO.
// hay un problema de eficiencia y creo varios ciclos.
// De hecho, creo número de tareas / 2 ciclos

// t1 << t2 y t2 << t1, ... t{n-1} << tn y tn << t{n-1}
 
// Para el fichero j30_1_1.dzn tenemos
//  n_tasks = 30;
 
// el fichero j30_1_1_fu_all_prec.dzn 
// suc = [ { 2 }, 
        // { 1 }, 
        // { 4 }, 
        // { 3 }, 
        // { 6 }, 
        // { 5 }, 
        // .......
        // { 30 }, 
        // { 29 }]; 


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
	string path_fu = "rcpsp\\fu\\M2\\j30\\J30";
	procesa_todos(path_file, path_fu);

	path_file = "rcpsp\\data_psplib\\j60\\J60";
	path_fu = "rcpsp\\fu\\M2\\j60\\J60";
	procesa_todos(path_file, path_fu);

	path_file = "rcpsp\\data_psplib\\j90\\J90";
	path_fu = "rcpsp\\fu\\M2\\j90\\J90";
	procesa_todos(path_file, path_fu);

	path_file = "rcpsp\\data_psplib\\j120\\J120";
	path_fu = "rcpsp\\fu\\M2\\j120\\J120";
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
	string aux = file_name + ".dzn";
	ifstream in(aux);
//	ofstream out(fu_name + "_fu_all_prec.dzn");   // OJO M1
	ofstream out(fu_name + "_fu_cycle.dzn");   // OJO M2

	std::string s = "";


	// Las líneas se leen y escriben directamente pq no cambian 
	// hasta encontrar una línea que comienza con suc

	std::string prefix = "suc";
	int num_tasks = 0;
	getline(in, s);
	while (s.compare(0, prefix.size(), prefix) != 0) {

		// Necesito saber el número de tareas pero está pegado al ; final

		// Trabajo con expresiones regulares para quitar = ; [ ]
		std::regex word_regex("(\\w+)");

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
		getline(in, s);
	}

	// Ahora construyo la sucesión de precedencias

	/* Si construyo un único ciclo de t1 << t2 y t2 << t1 tarda mucho 
	out << "suc = [ { 2 }, " << endl;
	out << "        { 1 }, " << endl;
	for (size_t i = 4; i <= num_tasks; i++) {
		out << "        {  }, " << endl;
	}
	out << "        {  }" << "]; " << endl;
	*/

	// Así que construyo más de un ciclo 
	//suc = [{ 2 },
	//{ 1 },
	//{ 4 },
	//{ 3 },
	//{ 6 },
	//{ 5 },
	//{ 8 },
	//{ 7 },
	//{ 10 },
	//{ 9 },
	//{ 12 },
	//{ 11 },
	//{ 14 },
	//{ 13 },
	//{ 16 },
	//{ 15 },
	//{ 18 },
	//{ 17 },
	//{ 20 },
	//{ 19 },
	//{ 22 },
	//{ 21 },
	//{ 24 },
	//{ 23 },
	//{ 26 },
	//{ 25 },
	//{ 28 },
	//{ 27 },
	//{ 30 },
	//{ 29 }];
	// y va superrrrrrr - rápido


	out << "suc = [ { 2 }, " << endl;
	for (size_t i = 2; i < num_tasks; i++) {
		if (i % 2 == 0) {
			out << "        { " << i-1 << " }, " << endl;
		}
		else {
			out << "        { " << i+1 << " }, " << endl;
		}
	}
	out << "        { " << num_tasks-1 << " }" << "]; " << endl;

	in.close();
}