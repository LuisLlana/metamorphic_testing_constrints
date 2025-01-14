#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <regex>
#include <unordered_map>

using namespace std;

struct tPaths {
	string file[100];
	int cont = 0;
};

struct tMutants_killed_matrix {
	int value[76][12] = { 0 };
	string name[76] = { "" };
	int ncol = 0;
};


struct tMutants_killed_matrix_2 {
	int value[76][481] = { 0 };
	string name_row[76] = { "" };
	string name_col[481] = { "" };
};


void mutants_paths(tPaths& array_mzns, tMutants_killed_matrix& matrix, tMutants_killed_matrix_2& matrix_2,
	tMutants_killed_matrix_2& matrix_MR1, tMutants_killed_matrix_2& matrix_MR2, tMutants_killed_matrix_2& matrix_MR3,
	tMutants_killed_matrix_2& matrix_MR4);

void mayor_durations_dicc(ifstream& file_mayor, unordered_map<string, int>& dicc_mayor_durations);

void sum_durations_dicc(ifstream& file_in, unordered_map<string, int>& dicc_durations);
void sum_durations(unordered_map<string, int>& dicc_durations, string original_mzn_original_dnz, int& sum_1,
	string original_mzn_fu_dzn, int& sum_2,
	string mut_mzn_original_dzn, int& sum_1_mut,
	string mut_mzn_fu_dzn, int& sum_2_mut);


void results_dicc(ifstream& file_in, unordered_map<string, int>& dicc_results);
bool check_files_and_find_Os(unordered_map<string, int>& dicc_results,
	string original_mzn_original_dnz, int& O1,
	string original_mzn_fu_dzn, int& O2,
	string mut_mzn_original_dzn, int& O1_mut,
	string mut_mzn_fu_dzn, int& O2_mut,
	ofstream& log);


string extract_dzn(string file);
string extract_mzn(string file);

void process_all(tPaths& array_mzns, tPaths& array_dzn, tMutants_killed_matrix& matrix, tMutants_killed_matrix_2& matrix_2, 
	tMutants_killed_matrix_2& matrix_MR1, tMutants_killed_matrix_2& matrix_MR2, tMutants_killed_matrix_2& matrix_MR3, 
	tMutants_killed_matrix_2& matrix_MR4);
bool process_MR1(ofstream& log, int O1, int O2, int O1_mut, int O2_mut, int sum_1, int sum_2, int sum_1_mut, int sum_2_mut);
bool process_MR2(ofstream& log, int O1, int O2, int O1_mut, int O2_mut);
bool process_MR3(ofstream& log, int O1, int O2, int O1_mut, int O2_mut);
bool process_MR4(ofstream& log, int O1, int O2, int O1_mut, int O2_mut);



int main() {

	// En el array de mzns, el primero es el código rcpsp.mzn original
	// y después todos los mzn mutantes
	tPaths array_mzns;
	tMutants_killed_matrix matrix;
	tMutants_killed_matrix_2 matrix_2, matrix_MR1, matrix_MR2, matrix_MR3, matrix_MR4;
	matrix_2.name_col[480] = "Result\n";
	matrix_MR1.name_col[480] = "Result\n";
	matrix_MR2.name_col[480] = "Result\n";
	matrix_MR3.name_col[480] = "Result\n";
	matrix_MR4.name_col[480] = "Result\n";


	mutants_paths(array_mzns, matrix, matrix_2, matrix_MR1, matrix_MR2, matrix_MR3, matrix_MR4);


	// En el array de dzns, el primero es el original dzn
	// y después todos los dzn generados como folow-up

	tPaths array_dzn;

	array_dzn.file[0] = ".dzn";   // ojo el primero es el original
	array_dzn.cont++;
	array_dzn.file[1] = "_fu_all_prec.dzn";
	array_dzn.cont++;
	array_dzn.file[2] = "_fu_cycle.dzn";
	array_dzn.cont++;
	array_dzn.file[3] = "_fu_d.dzn";
	array_dzn.cont++;
	array_dzn.file[4] = "_fu_rc_1.dzn";
	array_dzn.cont++;

	process_all(array_mzns, array_dzn, matrix, matrix_2, matrix_MR1, matrix_MR2, matrix_MR3, matrix_MR4);

	return 0;
}

void process_all(tPaths& array_mzns, tPaths& array_dzn, tMutants_killed_matrix& matrix, tMutants_killed_matrix_2& matrix_2,
	tMutants_killed_matrix_2& matrix_MR1, tMutants_killed_matrix_2& matrix_MR2, tMutants_killed_matrix_2& matrix_MR3, 
	tMutants_killed_matrix_2& matrix_MR4 ) {

	// Guardo el fichero "sum_durations.txt" en un diccionario

	ifstream file_in("sum_durations.txt");
	if (!file_in) {
		cout << " Fichero sum_durations.txt no encontrado. \n";
	}
	unordered_map<string, int> dicc_durations;
	sum_durations_dicc(file_in, dicc_durations);
	file_in.close();


	ifstream file_mayor("mayor_duration.txt");
	if (!file_mayor) {
		cout << " Fichero mayor_duration.txt no encontrado. \n";
	}
	unordered_map<string, int> dicc_mayor_duration;
	mayor_durations_dicc(file_mayor, dicc_mayor_duration);
	file_mayor.close();


	ifstream file_results("results_230818_231216.txt");
	if (!file_results) {
		cout << "Fichero results_230818_231216.txt no encontrado.  \n";
	}
	unordered_map<string, int> dicc_results;
	results_dicc(file_results, dicc_results);
	file_results.close();

	string s = "";
	ofstream log("V8_process_MRs_log.txt");

	ofstream mutants_killed("V8_mutants_killed.csv");
	mutants_killed << "Mutant;O1!=O1_mut;MR1;MR2;MR3;MR4;Existe diferencia;Killed\n";

	ofstream mutants_killed_2("V8_killed_different_outputs.csv");
	ofstream mutants_killed_MR1("V8_killed_MR1.csv");
	ofstream mutants_killed_MR2("V8_killed_MR2.csv");
	ofstream mutants_killed_MR3("V8_killed_MR3.csv");
	ofstream mutants_killed_MR4("V8_killed_MR4.csv");

	int cont_log = 0;
	int cont_killed, cont_killed_2 = 0, cont_killed_MR1 = 0, cont_killed_MR2 = 0, cont_killed_MR3 = 0, cont_killed_MR4 = 0;
	int ncol_value = 0;

	for (int i = 1; i <= 48; i++) {  // 1 -- 48
		for (int j = 1; j <= 10; j++) {  // 1 -- 10
			matrix_2.name_col[ncol_value] = "J30_" + to_string(i) + "_" + to_string(j) + array_dzn.file[0] + ";";
			matrix_MR1.name_col[ncol_value] = "J30_" + to_string(i) + "_" + to_string(j) + "_fu_all_prec" + array_dzn.file[0] + ";";
			matrix_MR2.name_col[ncol_value] = "J30_" + to_string(i) + "_" + to_string(j) + "_fu_cycle" + array_dzn.file[0] + ";";
			matrix_MR3.name_col[ncol_value] = "J30_" + to_string(i) + "_" + to_string(j) + "_fu_d" + array_dzn.file[0] + ";";
			matrix_MR4.name_col[ncol_value] = "J30_" + to_string(i) + "_" + to_string(j) + "_fu_rc_1" + array_dzn.file[0] + ";";


			cont_killed_2 = 0, cont_killed_MR1 = 0, cont_killed_MR2 = 0, cont_killed_MR3 = 0, cont_killed_MR4 = 0;

			for (size_t k = 0; k < array_mzns.cont; k++) {  //  k = 0  son los mutantes
				// Enfrento al dzn original [0] con cada uno de los fu generados [1],...,[6]
				for (size_t w = 1; w < array_dzn.cont; w++) {  // w = 1

					//               string path_aux = "out_T30_231216/rcpsp.mzn--J30";
					string path_aux = "rcpsp.mzn--J30";

					string original_mzn_original_dnz = path_aux + "_" + to_string(i) + "_" + to_string(j) + array_dzn.file[0];
					string original_mzn_fu_dzn = path_aux + "_" + to_string(i) + "_" + to_string(j) + array_dzn.file[w];

					string mut_mzn_original_dzn = array_mzns.file[k] + "_" + to_string(i) + "_" + to_string(j) + array_dzn.file[0];
					string mut_mzn_fu_dzn = array_mzns.file[k] + "_" + to_string(i) + "_" + to_string(j) + array_dzn.file[w];

					cont_log++;
					log << cont_log << " \n";

					int O1 = -3, O2 = -3, O1_mut = -3, O2_mut = -3;
					// -2 es TIMEOUT
					// -1 es UNSATISFIABLE
					// un valor >= 0 corresponde al makespan

					if (check_files_and_find_Os(dicc_results,
						original_mzn_original_dnz, O1, original_mzn_fu_dzn, O2,
						mut_mzn_original_dzn, O1_mut, mut_mzn_fu_dzn, O2_mut, log)) {


						log << "O1 " << original_mzn_original_dnz << " = " << O1 << " \n"
							<< "O2 " << original_mzn_fu_dzn << " = " << O2 << " \n"
							<< "O1_mut " << mut_mzn_original_dzn << " = " << O1_mut << " \n"
							<< "O2_mut " << mut_mzn_fu_dzn << " = " << O2_mut << " \n";

						cont_killed = 0;
						
						cont_killed_2 = 0, cont_killed_MR1 = 0, cont_killed_MR2 = 0, cont_killed_MR3 = 0, cont_killed_MR4 = 0;


						if (O1 == O1_mut && O2 == O2_mut) {
							log << "Equivalentes. Los resultados de los originales y mutantes son iguales.\n"
								<< "tanto en el log (O1 y O1_mut) como en los fu (O2 y O2_mut).";
						}
						else {

							// Se marcan como distintos. columna "Existe diferencia"
							matrix.value[k][5] = 1;

							// Si el original y el mutante tienen resultados distintos, se mata al mutante
							// Incluyendo insat (-1) y timeout (-2)
							if (O1 != O1_mut) {
								log << "O1 != O1_mut mata a " << matrix.name[k] << "\n";
								matrix.value[k][0] = 1;
								matrix_2.value[k][ncol_value] = 1;
								cont_killed++;
								cont_killed_2++;
							}

							// Para MR1 necesito las sumas
							int sum_1 = -3, sum_2 = -3, sum_1_mut = -3, sum_2_mut = -3;

							sum_durations(dicc_durations, original_mzn_original_dnz, sum_1, original_mzn_fu_dzn, sum_2,
								mut_mzn_original_dzn, sum_1_mut, mut_mzn_fu_dzn, sum_2_mut);

							string mut_1 = extract_dzn(mut_mzn_original_dzn);
							string mut_2 = extract_dzn(mut_mzn_original_dzn);
							int dur_max_1 = dicc_mayor_duration.at(mut_1);
							int dur_max_2 = dicc_mayor_duration.at(mut_2);

							// Proceso MR1 si le corresponde el fu (w==1) y si no ha sido
							// ya matado anteriormente
							//if (w == 1 && matrix.value[k][1] == 0) {
							if (w == 1) {

								if (process_MR1(log, O1, O2, O1_mut, O2_mut, sum_1, sum_2, sum_1_mut, sum_2_mut)) {
									matrix.value[k][1] = 1;
									matrix_MR1.value[k][ncol_value] = 1;

									cont_killed++;
									cont_killed_MR1++;

									log << "MR1 mata a " << matrix.name[k] << endl;
									// ":  O1: " << O1 << ", O2: " << O2
									//	<< ", O1_mut: " << O1_mut << ", O2_mut: " << O2_mut << ", sum_1 " << sum_1
									//	<< ", sum_2 " << sum_2 << ", sum_1_mut " << sum_1_mut << ", sum_2_mut " << sum_2_mut << endl;
								}
							}

							//if (w == 2 && matrix.value[k][2] == 0) {
							if (w == 2) {

								if (process_MR2(log, O1, O2, O1_mut, O2_mut)) {
									matrix.value[k][2] = 1;
									matrix_MR2.value[k][ncol_value] = 1;

									cont_killed++;
									cont_killed_MR2++;

									log << "MR2 mata a " << matrix.name[k] << endl;
									//  << ":  O1:" << O1 << ", O2: " << O2
									//	<< ", O1_mut: " << O1_mut << ", O2_mut: " << O2_mut << endl;
								}
							}

							//if (w == 3 && matrix.value[k][3] == 0) {
							if (w == 3) {

								if (process_MR3(log, O1, O2, O1_mut, O2_mut)) {
									matrix.value[k][3] = 1;
									matrix_MR3.value[k][ncol_value] = 1;

									cont_killed++;
									cont_killed_MR3++;

									log << "MR3 mata a " << matrix.name[k] << endl;
									//  << ":  O1:" << O1 << ", O2: " << O2
									//	<< ", O1_mut: " << O1_mut << ", O2_mut:" << O2_mut << endl;
								}
							}

							//if (w == 4 && matrix.value[k][4] == 0) {
							if (w == 4) {

								if (process_MR4(log, O1, O2, O1_mut, O2_mut)) {
									matrix.value[k][4] = 1;
									matrix_MR4.value[k][ncol_value] = 1;

									cont_killed++;
									cont_killed_MR4++;

									log << "MR4 mata a " << matrix.name[k] << endl;
									//  << ": O1:" << O1 << ", O2: " << O2
									//	<< ", O1_mut: " << O1_mut << ", O2_mut:" << O2_mut << endl;
								}
							}
						}
						if (cont_killed > 0) {
							matrix.value[k][6] = 1;  //  es la columna de Killed
						}
						if (cont_killed_2 > 0) {
							matrix_2.value[k][480] = 1;  //  es la columna de Killed
						}
						if (cont_killed_MR1 > 0) {
							matrix_MR1.value[k][480] = 1;  //  es la columna de Killed
						}
						if (cont_killed_MR2 > 0) {
							matrix_MR2.value[k][480] = 1;  //  es la columna de Killed
						}
						if (cont_killed_MR3 > 0) {
							matrix_MR3.value[k][480] = 1;  //  es la columna de Killed
						}
						if (cont_killed_MR4 > 0) {
							matrix_MR4.value[k][480] = 1;  //  es la columna de Killed
						}
					}
					log << "\n";
				}
			}

			ncol_value++;  //  El ˙ltimo (result) los pongo ad-hoc

		}
	}

	// Paso la matriz al fichero V8_mutants_killed.csv
	for (int nrow = 0; nrow < 76; nrow++) {
		mutants_killed << matrix.name[nrow];

		for (int ncol = 0; ncol < 7; ncol++) {
			mutants_killed << ";" << matrix.value[nrow][ncol];
		}
		mutants_killed << "\n";
	}

	// Paso la matriz al fichero V8_mutants_killed_diff_outputs.csv


	mutants_killed_2 << "Mutant;";
	mutants_killed_MR1 << "Mutant;";
	mutants_killed_MR2 << "Mutant;";
	mutants_killed_MR3 << "Mutant;";
	mutants_killed_MR4 << "Mutant;";

	for (int j = 0; j < 481; j++) {
		mutants_killed_2 << matrix_2.name_col[j];
		mutants_killed_MR1 << matrix_MR1.name_col[j];
		mutants_killed_MR2 << matrix_MR2.name_col[j];
		mutants_killed_MR3 << matrix_MR3.name_col[j];
		mutants_killed_MR4 << matrix_MR4.name_col[j];
	}

	for (int nrow = 0; nrow < 76; nrow++) {
		mutants_killed_2 << matrix_2.name_row[nrow];
		mutants_killed_MR1 << matrix_MR1.name_row[nrow];
		mutants_killed_MR2 << matrix_MR2.name_row[nrow];
		mutants_killed_MR3 << matrix_MR3.name_row[nrow];
		mutants_killed_MR4 << matrix_MR4.name_row[nrow];

		for (int ncol = 0; ncol < 481; ncol++) {
			mutants_killed_2 << ";" << matrix_2.value[nrow][ncol];
			mutants_killed_MR1 << ";" << matrix_MR1.value[nrow][ncol];
			mutants_killed_MR2 << ";" << matrix_MR2.value[nrow][ncol];
			mutants_killed_MR3 << ";" << matrix_MR3.value[nrow][ncol];
			mutants_killed_MR4 << ";" << matrix_MR4.value[nrow][ncol];
		}
		mutants_killed_2 << "\n";
		mutants_killed_MR1 << "\n";
		mutants_killed_MR2 << "\n";
		mutants_killed_MR3 << "\n";
		mutants_killed_MR4 << "\n";
	}




	// Escribo en el log los mutantes que no se han podido matar

	log << "\n\nMutantes que no se han podido matar \n";
	for (int nrow = 0; nrow < 76; nrow++) {
		if (matrix.value[nrow][6] == 0) {
			log << matrix.name[nrow] << "\n";
		}
	}
}

bool process_MR1(ofstream& log, int O1, int O2, int O1_mut, int O2_mut, int sum_1, int sum_2, int sum_1_mut, int sum_2_mut) {

	if (O1_mut == -1) {     // O1_mut debe ser finito (!= -1) 
		// log << "Este mutante no se puede tratar con MR1 porque O1_mut es UNSATISFIABLE. \n";
		return false;
	}

	if (O1_mut == -2) {     // O1_mut debe ser finito (!= -2) 
		// log << "Este mutante no se puede tratar con MR1 porque O1_mut es TIMEOUT. \n";
		return false;
	}

	if (O2_mut != sum_2_mut) {
		log << "Se puede matar con MR1, porque O2_mut (" << O2_mut << ") es distinto que la suma de las duraciones (" << sum_2_mut << "). \n";
		return true;
	}

	log << "Este mutante NO se puede matar con MR1. La suma de las duraciones (" << sum_2_mut << ") \n";
	return false;
}

bool process_MR2(ofstream& log, int O1, int O2, int O1_mut, int O2_mut) {

	if (O1_mut == -1) {   // O1_mut debe ser finito (!= -1) 
		//log << "Este mutante no se puede tratar con MR2 porque O1_mut es UNSATISFIABLE. \n";
		return false;
	}

	if (O1_mut == -2) {     // O1_mut debe ser finito (!= -2) 
		// log << "Este mutante no se puede tratar con MR2 porque O1_mut es TIMEOUT. \n";
		return false;
	}

	if (O2_mut != -1) {    // Si fu es finito (!= -1), entonces se mata
		log << "Este mutante SI se puede matar con MR2 pq su fu es finito (!= -1). \n";
		return true;
	}


	//log << "Este mutante NO se puede matar con MR2. \n";
	return false;
}

bool process_MR3(ofstream& log, int O1, int O2, int O1_mut, int O2_mut) {

	if (O1_mut == -1) {    // O1_mut debe ser finito (!= -1) 
		//log << "Este mutante no se puede tratar con MR3 porque O1_mut es UNSATISFIABLE. \n";
		return false;
	}

	if (O1_mut == -2) {    // O1_mut debe ser finito (!= -1) 
		//log << "Este mutante no se puede tratar con MR3 porque O1_mut es TIMEOUT. \n";
		return false;
	}

	if (O1_mut >= 0 && O2_mut <= O1_mut) {    // Si O1_mut < O2_mut (menor estricto) NO se cumple, entonces se mata
		log << "Este mutante SI se puede matar con MR3. \n";
		return true;
	}

	//log << "Este mutante NO se puede matar con MR3. \n";
	return false;
}

bool process_MR4(ofstream& log, int O1, int O2, int O1_mut, int O2_mut) {


	if (O1_mut == -1) {    // O1_mut debe ser finito (!= -1) 
		//log << "Existe un UNSATISFIABLE en O1_mut o O2_mut. \n";
		return false;
	}

	if (O1_mut == -2) {     // O1_mut debe ser finito (!= -2) 
		// log << "Este mutante no se puede tratar con MR4 porque O1_mut es TIMEOUT. \n";
		return false;
	}

	if (O2_mut == -2) {    // Si O2_mut <= O1_mut NO se cumple, entonces se mata
		log << "Este mutante se puede matar con MR4 pq O2_mut == -2. \n";
		return true;
	}

	if (O2_mut > O1_mut) {    // Si O2_mut <= O1_mut NO se cumple, entonces se mata
		log << "Este mutante se puede matar con MR4 pq O2_mut > O1_mut. \n";
		return true;
	}

	//log << "Este mutante NO se puede matar con MR4. \n";
	return false;
}

string extract_dzn(string file) {
	// string.substr(size_t begin, size_t length)
	unsigned long int pos1 = file.find("--");
	string file_aux = file.substr(pos1 + 2, 60);  //

	return file_aux;
}


string extract_mzn(string file) {
	unsigned long int pos1 = file.find("/");
	unsigned long int pos2 = file.find("--");
	string file_aux = file.substr(pos1 + 1, pos2 - pos1 - 1);  //

	file_aux.pop_back();  // t
	file_aux.pop_back();  // u
	file_aux.pop_back();  // o
	file_aux.pop_back();  // .
	file_aux.pop_back();  // n
	file_aux.pop_back();  // z
	file_aux.pop_back();  // m
	file_aux.pop_back();  // .

	return file_aux;
}

void sum_durations_dicc(ifstream& file_in, unordered_map<string, int>& dicc_durations) {
	// El fichero sum_durations.txt tiene nombre de fichero y su suma
	string aux;
	int sum;

	file_in >> aux >> sum;
	while (file_in) {
		dicc_durations[aux] = sum;
		file_in >> aux >> sum;
	}
}


void mayor_durations_dicc(ifstream& file_mayor, unordered_map<string, int>& dicc_mayor_durations) {

	string aux;
	int sum;

	file_mayor >> aux >> sum;
	while (file_mayor) {
		dicc_mayor_durations[aux] = sum;
		file_mayor >> aux >> sum;
	}
}


void sum_durations(unordered_map<string, int>& dicc_durations,
	string original_mzn_original_dnz, int& sum_1, string original_mzn_fu_dzn, int& sum_2,
	string mut_mzn_original_dzn, int& sum_1_mut, string mut_mzn_fu_dzn, int& sum_2_mut) {

	// Extraigo el fichero dzn

	original_mzn_original_dnz = extract_dzn(original_mzn_original_dnz);
	original_mzn_fu_dzn = extract_dzn(original_mzn_fu_dzn);
	mut_mzn_original_dzn = extract_dzn(mut_mzn_original_dzn);
	mut_mzn_fu_dzn = extract_dzn(mut_mzn_fu_dzn);


	if (dicc_durations.count(original_mzn_original_dnz) > 0) {
		sum_1 = dicc_durations.at(original_mzn_original_dnz);
	}
	else {
		cout << "La entrada: " << original_mzn_original_dnz << " NO existe en el diccionario dicc_durations" << endl;
	}

	if (dicc_durations.count(original_mzn_fu_dzn) > 0) {
		sum_2 = dicc_durations.at(original_mzn_fu_dzn);

	}
	else {
		cout << "La entrada: " << original_mzn_fu_dzn << " NO existe en el diccionario dicc_durations" << endl;
	}

	if (dicc_durations.count(mut_mzn_original_dzn) > 0) {
		sum_1_mut = dicc_durations.at(mut_mzn_original_dzn);
	}
	else {
		cout << "La entrada: " << mut_mzn_original_dzn << " NO existe en el diccionario dicc_durations" << endl;
	}

	if (dicc_durations.count(mut_mzn_fu_dzn) > 0) {
		sum_2_mut = dicc_durations.at(mut_mzn_fu_dzn);
	}
	else {
		cout << "La entrada: " << mut_mzn_fu_dzn << " NO existe en el diccionario dicc_durations" << endl;
	}
}


void results_dicc(ifstream& file_results, unordered_map<string, int>& dicc_results) {

	string s;
	int n;
	file_results >> s >> n;

	while (file_results) {
		unsigned long int pos1 = s.find("/");
		unsigned long int pos2 = s.find(".out");
		string aux = s.substr(pos1 + 1, pos2 - pos1 - 1);  //
		dicc_results[aux] = n;

		if (aux == "")
			cout << s << " -- " << n << endl;

		file_results >> s >> n;
	}
}

bool check_files_and_find_Os(unordered_map<string, int>& dicc_results,
	string original_mzn_original_dnz, int& O1,
	string original_mzn_fu_dzn, int& O2,
	string mut_mzn_original_dzn, int& O1_mut,
	string mut_mzn_fu_dzn, int& O2_mut,
	ofstream& log) {

	bool result = true;

	if (dicc_results.count(original_mzn_original_dnz) > 0) {
		O1 = dicc_results.at(original_mzn_original_dnz);
	}
	else {
		log << original_mzn_original_dnz << " NO encontrado. \n";
		result = false;
	}

	if (dicc_results.count(original_mzn_fu_dzn) > 0) {
		O2 = dicc_results.at(original_mzn_fu_dzn);
	}
	else {
		log << original_mzn_fu_dzn << " NO encontrado. \n";
		result = false;
	}

	if (dicc_results.count(mut_mzn_original_dzn) > 0) {
		O1_mut = dicc_results.at(mut_mzn_original_dzn);
	}
	else {
		log << mut_mzn_original_dzn << " NO encontrado. \n";
		result = false;
	}

	if (dicc_results.count(mut_mzn_fu_dzn) > 0) {
		O2_mut = dicc_results.at(mut_mzn_fu_dzn);
	}
	else {
		log << mut_mzn_fu_dzn << " NO encontrado. \n";
		result = false;
	}

	return result;
}


void mutants_paths(tPaths& array_mzns, tMutants_killed_matrix& matrix, tMutants_killed_matrix_2& matrix_2,
	tMutants_killed_matrix_2& matrix_MR1, tMutants_killed_matrix_2& matrix_MR2, tMutants_killed_matrix_2& matrix_MR3, 
	tMutants_killed_matrix_2& matrix_MR4) {

	ifstream file_in("mutants.txt");
	string aux = "";

	file_in >> aux;

	if (!file_in) {
		cout << "Fichero mutants.txt no encontrado  \n";
	}

	int i = 0;
	while (file_in) {
		//		array_mzns.file[array_mzns.cont] = "out_T300_230818/" + aux + "--J30";
		array_mzns.file[array_mzns.cont] = aux + "--J30";
		array_mzns.cont++;

		aux.pop_back(); // n
		aux.pop_back(); // z
		aux.pop_back(); // m
		aux.pop_back(); // .

		matrix.name[i] = aux;
		matrix_2.name_row[i] = aux;
		matrix_MR1.name_row[i] = aux;
		matrix_MR2.name_row[i] = aux;
		matrix_MR3.name_row[i] = aux;
		matrix_MR4.name_row[i] = aux;
		i++;

		file_in >> aux;
	}
	file_in.close();
}
