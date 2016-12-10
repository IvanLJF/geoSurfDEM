
/*
#
# data_decimation_01.cpp
# by Mauro Alberti - www.malg.eu, alberti.m65@gmail.com
# 2010-09-14
#
# This program or module is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 2 of the License, or
# version 3 of the License, or (at your option) any later version. It is
# provided for educational purposes and is distributed in the hope that
# it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
# the GNU General Public License for more details.
#
*/


#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>

using namespace std;

int isfloatpart (char c)
{
    if ((c >= '0' && c <='9') or (c == '.'))
        return 1;
    else
        return 0;
}

int main ()
{

    char comma;

    cout << "\n\nData Decimation\n";
    cout << "by M. Alberti - www.malg.eu\n";
    cout << "2010-09-14\n\n\n";


    // input of parameter file name and file opening
    cout << "Enter input parameter file name: ";
  	string param_filename;
    cin >> param_filename;

    ifstream param_file;
    param_file.open(param_filename.c_str(), ios::binary);

    if (param_file.fail())
    {
    cout << "\n\nUnable to open parameter file '" << param_filename << "'\n";
    return 1;
    }

    // start time
    clock_t start_time = clock();


    // parameter reading

    string data_filename_in, filtered_elev_flnm_in, grid_elevnum_flnm_in, cell_size_str_in;
    string data_filename, filtered_elev_flnm, grid_elevnum_flnm, cell_size_str;

    cout << "\nParameters\n";
    getline(param_file, data_filename_in); istringstream df(data_filename_in); df >> data_filename; cout << " - elevation data (input): " << data_filename << "\n";
    getline(param_file, filtered_elev_flnm_in); istringstream fe(filtered_elev_flnm_in); fe >> filtered_elev_flnm; cout << " - filtered elevations (output): " << filtered_elev_flnm << "\n";
    getline(param_file, grid_elevnum_flnm_in); istringstream ge(grid_elevnum_flnm_in); ge >> grid_elevnum_flnm; cout << " - grid of record number (output): " << grid_elevnum_flnm << "\n";
    getline(param_file, cell_size_str_in); istringstream cs(cell_size_str_in); cs >> cell_size_str; cout << " - cell size: " << cell_size_str << "\n";

    // tests cell size value being number
    for (int i = 0; i < cell_size_str.size(); i++)
    {
        if (isfloatpart(cell_size_str[i]) == 0)
        {
            cout << "\n\nCell size input: not correct\n";
            return 1;
        }
    }
    float cell_size;
	istringstream instr(cell_size_str);
	instr >> cell_size;

    ifstream infile;
    infile.open(data_filename.c_str(), ios::binary);

    if (infile.fail())
    {
    cout << "\n\nUnable to open input file '" << data_filename << "'\n";
    return 1;
    }

    ofstream outfile;
    outfile.open(filtered_elev_flnm.c_str(), ios::binary);

    if (outfile.fail())
    {
    cout << "\n\nUnable to create output file '" << filtered_elev_flnm << "'\n";
    return 1;
    }

    ofstream outgridfile;
    outgridfile.open(grid_elevnum_flnm.c_str(), ios::binary);

    if (outgridfile.fail())
    {
        cout << "\n\nUnable to create output grid file '" <<  grid_elevnum_flnm << "'\n";
        return 1;
    }



    //
    // processing begins
    //


    cout << "\n\n -- processing started ... please wait\n\n";

    string rec_line;

    //  list of raw data strings
    list<string> rawdata_list;

    // reads file header
    getline(infile, rec_line);

    while (!infile.eof())
    {
        getline(infile, rec_line);
        if (rec_line.size() > 0)
        {
            rawdata_list.push_back(rec_line);
        }

    }

    infile.close();

    int num_recs = rawdata_list.size();


    list<string>::iterator string_pos; // string list iterator

    vector<int> id(num_recs); vector<double> x(num_recs), y(num_recs), z(num_recs);

    int ndx = 0;

    for (string_pos=rawdata_list.begin(); string_pos!=rawdata_list.end(); string_pos++)
    {

        istringstream instr(*string_pos);
        instr >> id[ndx] >> comma >> x[ndx] >> comma >> y[ndx] >> comma >> z[ndx];
        ndx++;

    }

    // spatial range: min and max of x, y
    double x_min = *min_element( x.begin(), x.end() );
    double x_max = *max_element( x.begin(), x.end() );
    double y_min = *min_element( y.begin(), y.end() );
    double y_max = *max_element( y.begin(), y.end() );

    cout << "The spatial range of data is:\n";
    cout << "  x_min: " << x_min << " x_max: " << x_max << "\n";
    cout << "  y_min: " << y_min << " y_max: " << y_max << "\n\n";

    float x_range = x_max -  x_min;
    float y_range = y_max -  y_min;

    int columns = int(x_range/cell_size) + 1;
    int rows = int(y_range/cell_size) + 1;

    cout << "Number of columns = " << columns << " number of rows = " << rows << "\n";


    // calculation of #grid_n# (linear index of grid) and #set_ndx_nonemptycell#, set of filled cells by index
   vector<int> grid_i(num_recs), grid_j(num_recs), grid_n(num_recs);
   set<int> set_ndx_nonemptycell;


    for (int k = 0; k < num_recs; k++)
    {
        grid_i[k] = int( (x[k]- x_min)/cell_size);
        grid_j[k] = int( (y[k]- y_min)/cell_size);
        grid_n[k] = (columns*grid_i[k]) + grid_j[k];
        set_ndx_nonemptycell.insert(grid_n[k]);
    }


   // calculation of #index_mapping#, representing the records for each non-empty cell
   map<int, vector<int> > index_mapping;

   set<int>::const_iterator pos;
   for(pos = set_ndx_nonemptycell.begin(); pos != set_ndx_nonemptycell.end(); ++pos)
    {
        vector<int> empy_vector;
        index_mapping[*pos] = empy_vector;
    }

    for (int k = 0; k < num_recs; k++)
    {

      index_mapping[grid_n[k]].push_back(k);

    }

    outfile << "id_rec, x, y, z\n";

    map<int, vector<int> >::iterator iter_map;
    for(iter_map = index_mapping.begin(); iter_map != index_mapping.end(); ++iter_map)
    {
       vector<int> vector_ndx = (*iter_map).second;
       vector<double> elevations_in_cell;
       for (int pi = 0; pi < vector_ndx.size(); pi++)
       {
           elevations_in_cell.push_back(z[vector_ndx[pi]]);
       }
       //int ndx_raw_median_elevation = ndx_median_value(elevations_in_cell);
       //int ndx_median_elevation = vector_ndx[ndx_raw_median_elevation];
       //outfile << id[ndx_median_elevation] << "," << x[ndx_median_elevation] << "," << y[ndx_median_elevation] << "," << z[ndx_median_elevation] << "\n";

    }

    // output of ascii grid storing number of recs per cell

    outgridfile << "ncols " << columns << "\n";
    outgridfile << "nrows " << rows << "\n";
    outgridfile << "xllcorner " << x_min << "\n";
    outgridfile << "yllcorner " << y_min << "\n";
    outgridfile << "cellsize " << cell_size << "\n";
    outgridfile << "nodata_value " << 0 << "\n";

    for (int j=(rows-1);j>=0;j--)
    {
        for (int i=0;i<columns;i++)
        {
            int n = (columns*i) + j;

            if (index_mapping.count(n) ==0)
            {
                outgridfile << "0 ";
            }
            else
            {
                int num_of_recs_in_cell = index_mapping[n].size();
                outgridfile << num_of_recs_in_cell << " ";
            }
        }

    }

    outgridfile.close();

    // end time
    clock_t end_time = clock();
    float diff_time = ((float)end_time - (float)start_time)/1000.0;  // run time

    printf ("\n\nProcessing completed in %.2lf seconds\n\n", diff_time );


    return 0;
}



