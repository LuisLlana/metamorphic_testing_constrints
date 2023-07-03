// Este programa calcula los fu correspondiente a M1 
// donde se disponen todas las tareas de forma secuencial
// t1 << t2 << t3 << ... << tn
// Para el fichero j30_1_1.dzn tenemos
//  n_tasks = 30;
 
// el fichero j30_1_1_fu_all_prec.dzn 
// suc = [ { 2 }, 
//         { 3 }, 
//         { 4 }, 
//        ........
//         { 29 }, 
//         { 30 }, 
//         {  }]; 


#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <regex>
using namespace std;

void procesa(string file_name, string fu_name);

struct tFiles {
	string nombre = "";
	string texto = "";
};

int main() {

	//	ifstream in("C:\\Users\\DSIC\\Downloads\\minizinc-benchmarks-master_12_6_2023\\minizinc-benchmarks-master\\rcpsp\\data_psplib\\j30\\j30_1_1.dzn");

	string s = "";
	
//	string path_file = "rcpsp\\data_psplib\\j30\\J30";
//	string path_fu = "rcpsp\\fu\\M1\\j30\\J30";

//	string path_file = "rcpsp\\data_psplib\\j60\\J60";
//	string path_fu = "rcpsp\\fu\\M1\\j60\\J60";

//	string path_file = "rcpsp\\data_psplib\\j90\\J90";
//	string path_fu = "rcpsp\\fu\\M1\\j90\\J90";

	string path_file = "rcpsp\\data_psplib\\j120\\J120";
	string path_fu = "rcpsp\\fu\\M1\\j120\\J120";


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
	ofstream out(fu_name + "_fu_all_prec.dzn"); 

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

	out << "suc = [ { 2 }, " << endl;
	for (size_t i = 3; i <= num_tasks; i++){
		out << "        { " << i << " }, " << endl;
	}
	out << "        {  }"  << "]; " << endl;

	in.close();
	out.close();
}