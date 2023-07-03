// Este programa calcula los fu correspondiente a M4 
// donde se incrementa el valor de los componentes de rc
// que corresponden a la cota de los recursos disponibles.
// En el paper anterior corresponde a Figura 6 donde 
// availableHandlers: pasa de 4 a 40

// Para el fichero j30_1_1.dzn tenemos
//  n_res = 4;
//  rc = [ 12, 13, 4, 12 ];
//  y se calculan n_res * 3 ficheros  (hago 3 pruebas +10, +20,+30)

// el fichero j30_1_1_fu_rc1_10.dzn con  rc = [ 22, 13, 4, 12 ];
// el fichero j30_1_1_fu_rc1_20.dzn con  rc = [ 32, 13, 4, 12 ];
// el fichero j30_1_1_fu_rc1_30.dzn con  rc = [ 42, 13, 4, 12 ];
// el fichero j30_1_1_fu_rc2_10.dzn con  rc = [ 12, 23, 4, 12 ];
// el fichero j30_1_1_fu_rc2_20.dzn con  rc = [ 12, 33, 4, 12 ];
// el fichero j30_1_1_fu_rc2_30.dzn con  rc = [ 12, 43, 4, 12 ];
// el fichero j30_1_1_fu_rc1_10.dzn con  rc = [ 12, 13, 14, 12 ];
// el fichero j30_1_1_fu_rc1_20.dzn con  rc = [ 12, 13, 24, 12 ];
// el fichero j30_1_1_fu_rc1_30.dzn con  rc = [ 12, 13, 34, 12 ];
// el fichero j30_1_1_fu_rc2_10.dzn con  rc = [ 12, 13, 4, 22 ];
// el fichero j30_1_1_fu_rc2_20.dzn con  rc = [ 12, 13, 4, 32 ];
// el fichero j30_1_1_fu_rc2_30.dzn con  rc = [ 12, 13, 4, 42 ];




#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <regex>
using namespace std;

void procesa(string file_name, string fu_name);
void procesa_todos(string file_name, string fu_name);

struct tFiles {
	string nombre = "";
	string texto = "";
};

int main() {

	string path_file = "rcpsp\\data_psplib\\j30\\J30";
	string path_fu = "rcpsp\\fu\\M4\\j30\\J30";
	procesa_todos(path_file, path_fu);

	path_file = "rcpsp\\data_psplib\\j60\\J60";
	path_fu = "rcpsp\\fu\\M4\\j60\\J60";
	procesa_todos(path_file, path_fu);

	path_file = "rcpsp\\data_psplib\\j90\\J90";
	path_fu = "rcpsp\\fu\\M4\\j90\\J90";
	procesa_todos(path_file, path_fu);

	path_file = "rcpsp\\data_psplib\\j120\\J120";
	path_fu = "rcpsp\\fu\\M4\\j120\\J120";
	procesa_todos(path_file, path_fu);
}

void procesa_todos(string path_file, string path_fu) {

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

	std::string s = "";

	// La primera línea me dice n_res = ? 
	int n_res = 0;
	getline(in, s);

	// Trabajo con expresiones regulares para quitar = ; [ ]
	std::regex word_regex("(\\w+)");

	// Uso iteradores al estilo https://runebook.dev/es/docs/cpp/regex?
	std::sregex_iterator i = std::sregex_iterator(s.begin(), s.end(), word_regex);
	std::smatch match = *i;
	std::string match_str = match.str();
	// No hace falta el if, pero lo dejo
	if (match_str.compare("n_res") == 0) {
		++i;
		match = *i;
		match_str = match.str();
		n_res = stoi(match_str);  // casting de string a int
	}

	// Hay que crear tantos fu como n_res

	tFiles* array_files = new tFiles[n_res];

	int cont1 = 0;
	// Todos los ficheros de salida tienen la misma línea 1: array_files[cont2].texto = s; 
	while (cont1 < n_res) {
		array_files[cont1].nombre = fu_name + "_fu_rc_" + to_string(cont1 + 1) + ".dzn";
		array_files[cont1].texto = s;
		cont1++;
	}

	// la segunda línea es de la forma rc = [...]
	getline(in, s);
	i = std::sregex_iterator(s.begin(), s.end(), word_regex);

	string* array_din_rc = new string[n_res];

	match = *i;
	match_str = match.str();
	if (match_str.compare("rc") == 0) {

		// Guardo los datos rc = [ 12, 13, 4, 12 ]; en un array dinámico array_din_rc
		for (int k = 0; k < n_res; k++) {
			++i;
			match = *i;
			match_str = match.str();
			array_din_rc[k] = match_str;
		}

		// Genero la salida
		cont1 = 0;
		//		int pos = 0, suma = 10;
		int* array_din_rc_aux = new int[n_res];
		while (cont1 < n_res) {
			string aux = "";
			aux += "\nrc = [ ";

			for (int k = 0; k < n_res; k++) {
				array_din_rc_aux[k] = stoi(array_din_rc[k]);
				if (k == cont1) {
					array_din_rc_aux[k] = 2 * array_din_rc_aux[k];
				}
				aux += to_string(array_din_rc_aux[k]);   // añado elemento a elemento
				if (k < n_res - 1) aux += ", ";
			}
			aux += " ];\n";
			array_files[cont1].texto += aux;
			cont1++;
		}
		delete[] array_din_rc_aux;
	}
	// El resto de líneas se leen y escriben directamente pq no cambian

	getline(in, s);
	while (in) {
		for (int i = 0; i < n_res; i++) {
			array_files[i].texto += s + "\n";
		}
		getline(in, s);
	}

	// Ahora paso toda la información que tengo en array_files
	// Por cada elemento del array array_files genero un fichero

	for (size_t i = 0; i < n_res; i++) {
		ofstream output_file(array_files[i].nombre);
		output_file << array_files[i].texto;
		output_file.close();
	}

	delete[] array_din_rc;
	delete[] array_files;
	in.close();
}