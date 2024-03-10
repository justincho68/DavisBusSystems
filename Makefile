CC = gcc
CXX = g++

INC_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
TEST_SRC_DIR = ./testsrc
TEST_TMP_DIR = ./testtmp

CXXFLAGS = -std=c++17 -I$(INC_DIR) #-Wall
LDFLAGS = -lgtest -lgtest_main -lpthread -lexpat

all: directories runtests

runtests: run_teststrutils run_teststrdatasource run_teststrdatasink run_testfiledatass

run_teststrutils: $(BIN_DIR)/teststrutils
	$(BIN_DIR)/teststrutils --gtest_output=xml:$(TEST_TMP_DIR)/run_teststrutils
	mv $(TEST_TMP_DIR)/run_teststrutils run_teststrutils

run_teststrdatasource: $(BIN_DIR)/teststrdatasource
	$(BIN_DIR)/teststrdatasource --gtest_output=xml:$(TEST_TMP_DIR)/run_teststrdatasource
	mv $(TEST_TMP_DIR)/run_teststrdatasource run_teststrdatasource

run_teststrdatasink: $(BIN_DIR)/teststrdatasink
	$(BIN_DIR)/teststrdatasink --gtest_output=xml:$(TEST_TMP_DIR)/run_teststrdatasink
	mv $(TEST_TMP_DIR)/run_teststrdatasink run_teststrdatasink

run_testfiledatass: $(BIN_DIR)/testfiledatass
	$(BIN_DIR)/testfiledatass --gtest_otuput=xml:$(TEST_TMP_DIR)/run_testfiledatass
	mv $(TEST_TMP_DIR)/run_testfiledatass run_testfiledatass

$(OBJ_DIR)/StringUtils.o: $(SRC_DIR)/StringUtils.cpp $(INC_DIR)/StringUtils.h
	@$(CXX) -o $(OBJ_DIR)/StringUtils.o -c $(CXXFLAGS) $(SRC_DIR)/StringUtils.cpp

$(OBJ_DIR)/StringUtilsTest.o: $(TEST_SRC_DIR)/StringUtilsTest.cpp $(INC_DIR)/StringUtils.h
	@$(CXX) -o $(OBJ_DIR)/StringUtilsTest.o -c $(CXXFLAGS) $(TEST_SRC_DIR)/StringUtilsTest.cpp

$(BIN_DIR)/teststrutils: $(OBJ_DIR)/StringUtils.o $(OBJ_DIR)/StringUtilsTest.o
	@$(CXX) -o $(BIN_DIR)/teststrutils $(CXXFLAGS) $(OBJ_DIR)/StringUtils.o $(OBJ_DIR)/StringUtilsTest.o $(LDFLAGS)

$(OBJ_DIR)/StringDataSource.o: $(SRC_DIR)/StringDataSource.cpp $(INC_DIR)/StringDataSource.h $(INC_DIR)/DataSource.h
	@$(CXX) -o $(OBJ_DIR)/StringDataSource.o -c $(CXXFLAGS) $(SRC_DIR)/StringDataSource.cpp

$(OBJ_DIR)/StringDataSourceTest.o: $(TEST_SRC_DIR)/StringDataSourceTest.cpp $(INC_DIR)/StringDataSource.h $(INC_DIR)/DataSource.h
	@$(CXX) -o $(OBJ_DIR)/StringDataSourceTest.o -c $(CXXFLAGS) $(TEST_SRC_DIR)/StringDataSourceTest.cpp

$(BIN_DIR)/teststrdatasource: $(OBJ_DIR)/StringDataSource.o $(OBJ_DIR)/StringDataSourceTest.o
	@$(CXX) -o $(BIN_DIR)/teststrdatasource $(CXXFLAGS) $(OBJ_DIR)/StringDataSource.o $(OBJ_DIR)/StringDataSourceTest.o $(LDFLAGS)

$(OBJ_DIR)/StringDataSink.o: $(SRC_DIR)/StringDataSink.cpp $(INC_DIR)/StringDataSink.h $(INC_DIR)/DataSource.h
	@$(CXX) -o $(OBJ_DIR)/StringDataSink.o -c $(CXXFLAGS) $(SRC_DIR)/StringDataSink.cpp

$(OBJ_DIR)/StringDataSinkTest.o: $(TEST_SRC_DIR)/StringDataSinkTest.cpp $(INC_DIR)/StringDataSink.h $(INC_DIR)/DataSource.h
	@$(CXX) -o $(OBJ_DIR)/StringDataSinkTest.o -c $(CXXFLAGS) $(TEST_SRC_DIR)/StringDataSinkTest.cpp

$(BIN_DIR)/teststrdatasink: $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/StringDataSinkTest.o
	@$(CXX) -o $(BIN_DIR)/teststrdatasink $(CXXFLAGS) $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/StringDataSinkTest.o $(LDFLAGS)

$(OBJ_DIR)/DSVReader.o: $(SRC_DIR)/DSVReader.cpp $(INC_DIR)/DSVReader.h
	@$(CXX) -o $(OBJ_DIR)/DSVReader.o -c $(CXXFLAGS) $(SRC_DIR)/DSVReader.cpp

$(OBJ_DIR)/DSVWriter.o: $(SRC_DIR)/DSVWriter.cpp $(INC_DIR)/DSVWriter.h
	@$(CXX) -o $(OBJ_DIR)/DSVWriter.o -c $(CXXFLAGS) $(SRC_DIR)/DSVWriter.cpp

$(OBJ_DIR)/DSVTest.o: $(TEST_SRC_DIR)/DSVTest.cpp $(INC_DIR)/DSVReader.h $(INC_DIR)/DSVWriter.h $(INC_DIR)/StringDataSink.h $(INC_DIR)/StringDataSource.h
	@$(CXX) -o $(OBJ_DIR)/DSVTest.o -c $(CXXFLAGS) $(TEST_SRC_DIR)/DSVTest.cpp

$(BIN_DIR)/testdsv: $(OBJ_DIR)/DSVReader.o $(OBJ_DIR)/DSVWriter.o $(OBJ_DIR)/DSVTest.o $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/StringDataSource.o
	@$(CXX) -o $(BIN_DIR)/testdsv $(CXXFLAGS) $(OBJ_DIR)/DSVReader.o $(OBJ_DIR)/DSVWriter.o $(OBJ_DIR)/DSVTest.o $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/StringDataSource.o $(LDFLAGS)

$(OBJ_DIR)/XMLReader.o: $(SRC_DIR)/XMLReader.cpp $(INC_DIR)/XMLReader.h
	@$(CXX) -o $(OBJ_DIR)/XMLReader.o -c $(CXXFLAGS) $(SRC_DIR)/XMLReader.cpp

$(OBJ_DIR)/XMLWriter.o: $(SRC_DIR)/XMLWriter.cpp $(INC_DIR)/XMLWriter.h
	@$(CXX) -o $(OBJ_DIR)/XMLWriter.o -c $(CXXFLAGS) $(SRC_DIR)/XMLWriter.cpp

$(OBJ_DIR)/XMLTest.o: $(TEST_SRC_DIR)/XMLTest.cpp $(INC_DIR)/XMLReader.h $(INC_DIR)/XMLWriter.h
	@$(CXX) -o $(OBJ_DIR)/XMLTest.o -c $(CXXFLAGS) $(TEST_SRC_DIR)/XMLTest.cpp

$(BIN_DIR)/testxml: $(OBJ_DIR)/XMLReader.o $(OBJ_DIR)/XMLWriter.o $(OBJ_DIR)/XMLTest.o $(OBJ_DIR)/StringDataSource.o $(OBJ_DIR)/StringDataSink.o
	@$(CXX) -o $(BIN_DIR)/testxml $(OBJ_DIR)/XMLReader.o $(OBJ_DIR)/XMLWriter.o $(OBJ_DIR)/XMLTest.o $(OBJ_DIR)/StringDataSource.o $(OBJ_DIR)/StringDataSink.o $(LDFLAGS)

#testcsvbs
$(OBJ_DIR)/CSVBusSystem.o: $(SRC_DIR)/CSVBusSystem.cpp $(INC_DIR)/CSVBusSystem.h
	@$(CXX) -o $(OBJ_DIR)/CSVBusSystem.o -c $(CXXFLAGS) $(SRC_DIR)/CSVBusSystem.cpp

$(OBJ_DIR)/CSVBusSystemTest.o: $(TEST_SRC_DIR)/CSVBusSystemTest.cpp $(INC_DIR)/CSVBusSystem.h
	@$(CXX) -o $(OBJ_DIR)/CSVBusSystemTest.o -c $(CXXFLAGS) $(TEST_SRC_DIR)/CSVBusSystemTest.cpp

$(BIN_DIR)/testcsvbs: $(OBJ_DIR)/CSVBusSystem.o $(OBJ_DIR)/CSVBusSystemTest.o $(OBJ_DIR)/DSVReader.o $(OBJ_DIR)/StringDataSource.o 
	@$(CXX) -o $(BIN_DIR)/testcsvbs $(OBJ_DIR)/CSVBusSystem.o $(OBJ_DIR)/CSVBusSystemTest.o $(OBJ_DIR)/DSVReader.o $(OBJ_DIR)/StringDataSource.o $(LDFLAGS)

#testosm
$(OBJ_DIR)/OpenStreetMap.o: $(SRC_DIR)/OpenStreetMap.cpp $(INC_DIR)/OpenStreetMap.h
	@$(CXX) -o $(OBJ_DIR)/OpenStreetMap.o -c $(CXXFLAGS) $(SRC_DIR)/OpenStreetMap.cpp

$(OBJ_DIR)/OpenStreetMapTest.o: $(TEST_SRC_DIR)/OpenStreetMapTest.cpp $(INC_DIR)/OpenStreetMap.h
	@$(CXX) -o $(OBJ_DIR)/OpenStreetMapTest.o -c $(CXXFLAGS) $(TEST_SRC_DIR)/OpenStreetMapTest.cpp

$(BIN_DIR)/testosm: $(OBJ_DIR)/OpenStreetMap.o $(OBJ_DIR)/OpenStreetMapTest.o $(OBJ_DIR)/XMLReader.o $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/StringDataSource.o
	@$(CXX) -o $(BIN_DIR)/testosm $(OBJ_DIR)/OpenStreetMap.o $(OBJ_DIR)/OpenStreetMapTest.o $(OBJ_DIR)/XMLReader.o $(OBJ_DIR)/StringDataSink.o $(OBJ_DIR)/StringDataSource.o $(LDFLAGS)

$(BIN_DIR)/testfiledatass: $(OBJ_DIR)/FileDataSource.o $(OBJ_DIR)/FileDataSink.o $(OBJ_DIR)/FileDataFactory.o $(OBJ_DIR)/FileDataSSTest.o
	@$(CXX) -o $(BIN_DIR)/testfiledatass $(OBJ_DIR)/FileDataSource.o $(OBJ_DIR)/FileDataSink.o $(OBJ_DIR)/FileDataFactory.o $(OBJ_DIR)/FileDataSSTest.o





directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(TEST_TMP_DIR)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
	rm -rf $(TEST_TMP_DIR)