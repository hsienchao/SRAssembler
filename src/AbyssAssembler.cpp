/*
 * AbyssAssembler.cpp
 *
 *  Created on: Oct 15, 2011
 *      Author: hchou
 */

#include "AbyssAssembler.h"


AbyssAssembler::AbyssAssembler(int log_level, string log_string):Assembler(log_level, log_string) {
}

AbyssAssembler::~AbyssAssembler() {
}

bool AbyssAssembler::is_available(){
	int ret = system("abyss-pe > /dev/null 2>&1");
	if (WEXITSTATUS(ret) != 0 && WEXITSTATUS(ret) != 2) {
		cout << "Cannot find ABySS, check your PATH variable!" << WEXITSTATUS(ret) << endl;
	    return false;
	}
	return true;
}
void AbyssAssembler::do_assembly(int kmer, const vector<Library>& libraries, const string& output_file)
{
	string lib_list = "";
	string paied_files = "";
	string single_files = "";
	for (unsigned int i=0; i<libraries.size();i++){
		Library lib = libraries[i];
		if (lib.get_paired_end()) {
			string lib_name = "pe" + int2str(lib.get_insert_size());
			lib_list +=  lib_name + " ";
			paied_files += lib_name + "='" + lib.get_matched_left_read_name() + " " + lib.get_matched_right_read_name() + "' ";
		} else
			single_files += lib.get_matched_left_read_name() + " ";
	}
	if (lib_list != ""){
	    lib_list = "lib='" + lib_list + "'";
	}
	single_files = "se='" + single_files + "'";
	string cmd = "abyss-pe contigs k=" + int2str(kmer) + " name=" + output_file + " " + lib_list + " " + paied_files + " " + single_files + ">> " + logger->get_log_file() + " 2>&1";
	logger->debug(cmd);
	run_shell_command(cmd);

}

void AbyssAssembler::clean_files(const string& dir){
	string cmd = "rm -rf " + dir + "/assembled*";
	logger->debug(cmd);
	run_shell_command(cmd);
}

string AbyssAssembler::get_output_contig_file_name(string prefix){
	int i=1;
	int best_i = 1;
	long max_size = 0;
	while (file_exists(prefix + "-" + int2str(i) + ".fa")){
		long fs = get_file_size(prefix + "-" + int2str(i) + ".fa");
		if (fs > max_size){
			max_size = fs;
			best_i = i;
		}
		i++;
	}
	//system(ls -l somefile assembled_15_3-contigs.fa | awk '{print $11}' > out.txt)
	return prefix + "-" + int2str(i-1) + ".fa";
	//return output_dir + "/assembled_" + int2str(k) + "_" + int2str(round) + "-contigs.fa";
}

string AbyssAssembler::get_output_scaffold_file_name(string prefix){
	return get_output_contig_file_name(prefix);
}
