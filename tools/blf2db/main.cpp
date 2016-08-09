/* main.cpp (blf2db) $Revision: 1.0 $ $Date: 2009/11/13 13:20:00 $
   Copyright (C) 2000 University of Bonn.
   http://www.ikp.uni-bonn.de/boss

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA  02111-1307, USA.
*/

/**
   @author Igor Jauk
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


// Local include files
#include "boss_utility/boss_config.h"
#include "boss_utility/boss_utility.h"
#include "boss_mysql/boss_mysql_handler.h"


static const char USAGE[]      = "blf2db -lex lexicon -table tablename (exception_lexicon | morpheme_lexicon) [-u user_name] [-h host_name] [-p password] [-db database] [-lang language_code]";
MySQL_Handler * db(0);


void insert (vector <string> & col, string & table, string & inv, string & lang, MySQL_Handler * db, MySQL_Result & query_result) {
	stringstream query;
	query << "INSERT INTO boss_" << lang << "_" << inv << "." << table << " SET ";
	for (int i=0; i<query_result.row(); i++) {
		if (i>0)
			query << ", ";
		query << query_result.operator ()(i,0) << "=\""
				<< col[i] << "\"";
	}
	try {
		db->query(query.str().c_str(), true);
	} catch (const MySQL_QueryFailed & e) {
		cerr << "Data: " << query.str() << endl;
		e.printError(true);
	}

}



int main(int argc, char *argv[]) {
	BOSS::Config cl(argc, argv, USAGE);
	string table, inv, lang, tmp;
	MySQL_Result query_result;
	stringstream query;

	try {
			db = new MySQL_Handler(cl("-h").optionarg,cl("-u").optionarg,cl("-p").optionarg,cl("-db").optionarg);
	} catch (const MySQL_ConnectFailed & e) {
			e.printError(true);
	}

	ifstream in(cl("-lex").optionarg.c_str());
	table = cl("-table").optionarg.c_str();	//table can be exception_lexicon or morpheme_lexicon
	inv = cl.getInvName();
	lang = cl.getLang();
	
	query << "truncate table boss_" << lang << "_" << inv << "." << table << ends;
	db->query(query.str().c_str());
	query.str("");
	query << "SHOW FIELDS IN " << table << " in boss_de_" << inv << ends;
	query_result = db->query(query.str().c_str());

	while(in) {
		getline (in, tmp);
		vector <string> col;
		stringstream line(tmp);


		while (line >> tmp)
			col.push_back(tmp);

		if (query_result.row() != col.size()) {
			if (col.size() == 0) {
				cerr << "done" << endl;
				exit(1);
			}
			cerr << "Error: the column number in the database table is not equal to the one in the input file. Table-"
					<< query_result.row() << ":File-" << col.size() << " Aborting." << endl;
			exit(-1);
		}

		insert (col, table, inv, lang, db, query_result);
	}

	cerr << "done" << endl;

	return 0;
}






