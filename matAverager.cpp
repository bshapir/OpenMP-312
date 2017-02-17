            #include <sys/time.h>
            #include <fstream>
            #include <iostream>
            #include <omp.h>
            #include <cstdlib>
            #include <sstream>
            #include <list>
            #include <string>

            using namespace std;

            // a class to get more accurate time

            class stopwatch{

            private:
                double elapsedTime;
                double startedTime;
                bool timing;
                //returns current time in seconds
                double current_time( )
                {
                    timeval tv;
                    gettimeofday(&tv, NULL);
                    double rtn_value = (double) tv.tv_usec;
                    rtn_value /= 1e6;
                    rtn_value += (double) tv.tv_sec;
                    return rtn_value;
                }

            public:
                stopwatch( ): elapsedTime( 0 ), startedTime( 0 ), timing( false )
                {

                }

                void start( )
                {
                    if( !timing )
                    {
                        timing = true;
                        startedTime = current_time( );
                    }
                }

                void stop( )
                {
                    if( timing )
                    {
                        elapsedTime +=  current_time( )-startedTime;
                        timing = false;
                    }
                }

                void resume( )
                {
                    start( );
                }

                void reset( )
                {
                    elapsedTime = 0;
                    startedTime = 0;
                    timing = false;
                }

                double getTime( )
                {
                    return elapsedTime;
                }
            };



            // function takes an array pointer, and the number of rows and cols in the array, and
            // allocates and intializes the two dimensional array to a bunch of random numbers

            void makeRandArray( unsigned int **& data, unsigned int rows, unsigned int cols, unsigned int seed )
            {
                // allocate the array
                data = new unsigned int*[ rows ];
                for( unsigned int i = 0; i < rows; i++ )
                {
                    data[i] = new unsigned int[ cols ];
                }

                // seed the number generator
                // you should change the seed to get different values
                srand( seed );

                // populate the array

                for( unsigned int i = 0; i < rows; i++ )
                    for( unsigned int j = 0; j < cols; j++ )
                    {
                        data[i][j] = rand() % 10000 + 1; // number between 1 and 10000
                    }
            }

            void getDataFromFile( unsigned int **& data, char fileName[], unsigned int &rows, unsigned int &cols )
            {
                ifstream in;
                in.open( fileName );
                if( !in )
                {
                    cerr << "error opening file: " << fileName << endl;
                    exit( -1 );
                }

                in >> rows >> cols;
                data = new unsigned int*[ rows ];
                for( unsigned int i = 0; i < rows; i++ )
                {
                    data[i] = new unsigned int[ cols ];
                }

                // now read in the data

                for( unsigned int i = 0; i < rows; i++ )
                    for( unsigned int j = 0; j < cols; j++ )
                    {
                        in >> data[i][j];
                    }

            }


            int main( int argc, char* argv[] )
            {
                if( argc < 3 )
                {
                    cerr<<"Usage: exe [input data file] [num of threads to use] " << endl;

                    cerr<<"or Usage: exe rand [num of threads to use] [num rows] [num cols] [seed value]" << endl;
                            exit( 0 );
                    }

                // read in the file
                unsigned int rows, cols, seed;
                unsigned int numThreads;
                unsigned int ** data;
                float * storAvg; //2D array to store data[i][j] average.
                //float avg = 0.0;
                float highAvg = 0.0;
                unsigned int highAvgLocation[2] = {0,0};
                // convert numThreads to int
                {
                    stringstream ss1;
                    ss1 << argv[2];
                    ss1 >> numThreads;
                }

                string fName( argv[1] );
                if( fName == "rand" )
                {
                    {
                        stringstream ss1;
                        ss1 << argv[3];
                        ss1 >> rows;
                    }
                    {
                        stringstream ss1;
                        ss1 << argv[4];
                        ss1 >> cols;
                    }
                    {
                        stringstream ss1;
                        ss1 << argv[5];
                        ss1 >> seed;
                    }
                    makeRandArray( data, rows, cols, seed );
                }
                else
                {
                    getDataFromFile( data,  argv[1], rows, cols );
                }
                omp_set_num_threads( numThreads );
                stopwatch S1;
                storAvg = new float[ rows*cols ];
                S1.start();

                #pragma omp parallel for
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < (cols); j++) {
                        if ((i>0) && (i<(rows-1))) { //not at top or bot
                            if ((j>0) && (j<(cols-1))) //not at top bot, not at left right?
                            {
                                storAvg[i*rows+j]=( ((data[i - 1][j - 1]) + (data[i - 1][j]) + (data[i - 1][j + 1]))
                                + ((data[i][j - 1]) + (data[i][j]) + (data[i][j + 1]))
                                + ((data[i + 1][j - 1]) + (data[i + 1][j]) + (data[i + 1][j + 1]))) /9.0;
                                //top left + top mid + top right ....mid.....bot /9
                            }
                            else if (j == 0) //not top, bot. at left?
                            {
                                storAvg[i*rows+j] =( ((data[i - 1][j]) + (data[i - 1][j + 1])) +
                                ((data[i][j]) + (data[i][j + 1])) + ((data[i + 1][j]) +
                                (data[i + 1][j + 1]))) /6.0;
                            }
                            else //otherwise not top, bot. at right.
                            {
                                storAvg[i*rows+j] =( ((data[i - 1][j - 1]) + (data[i - 1][j])) +
                                ((data[i][j - 1]) + (data[i][j])) +
                                ((data[i + 1][j - 1]) + (data[i + 1][j])) ) / 6.0;
                            }
                        }
                        else if (i == 0) { // at top (first row)
                            if ((j>0) && (j<(cols-1)))
                            {
                                storAvg[i*rows+j] =( ((data[i][j - 1]) + (data[i][j]) + (data[i][j + 1])) +
                                ((data[i + 1][j - 1]) + (data[i + 1][j]) + (data[i + 1][j + 1])) ) / 6.0;
                            }
                            else if (j == 0)
                            {
                                storAvg[i*rows+j] =( ((data[i][j]) + (data[i][j + 1])) +
                                ((data[i + 1][j]) + (data[i + 1][j + 1])) )/4.0;
                            }
                            else
                            {
                                storAvg[i*rows+j] =( ((data[i][j - 1]) + (data[i][j])) +
                                ((data[i + 1][j - 1]) + (data[i + 1][j])) ) /4.0;
                            }
                        }
                        else //otherwise you're at bot (last row)
                            {
                            if ((j>0) && (j<(cols-1)))
                            {
                                storAvg[i*rows+j] =( ((data[i - 1][j - 1]) + (data[i - 1][j]) + (data[i - 1][j + 1])) +
                                ((data[i][j - 1]) + (data[i][j]) + (data[i][j + 1]))) / 6.0;
                            }
                            else if (j == 0)
                            {
                                storAvg[i*rows+j] =( ((data[i - 1][j]) + (data[i - 1][j + 1])) +
                                ((data[i][j]) + (data[i][j + 1])) ) /4.0;
                            }
                            else //if (j == (cols-1))
                            {
                                storAvg[i*rows+j] =( ((data[i - 1][j - 1]) + (data[i - 1][j])) +
                                ((data[i][j - 1]) + (data[i][j])) ) /4.0;
                            }
                        }
                    }
            }
            //traverse averages and find highest loc.
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < (cols); j++) {
                    if (storAvg[i*rows+j] > highAvg) {
                        highAvg = storAvg[i*rows+j];
                        highAvgLocation[0] = i;
                        highAvgLocation[1] = j;
                    }
                }
            }
            S1.stop();
            cout << "largest average: " << highAvg << endl;
            cout << "found at cell: (" << highAvgLocation[0] << "," << highAvgLocation[1] << ")" << endl;
            cerr << "elapsed time: " << S1.getTime() << endl;
            //for (int i = 0; i< rows; i++) {
              //  delete storAvg[i];
            //}
           // delete storAvg;
            for (int i = 0; i< rows; i++) {
                delete data[i];
            }
            delete data;
        }


