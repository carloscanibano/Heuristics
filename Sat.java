//package src;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;

import org.jacop.core.BooleanVar;
import org.jacop.core.Store;
import org.jacop.jasat.utils.structures.IntVec;
import org.jacop.satwrapper.SatWrapper;
import org.jacop.search.DepthFirstSearch;
import org.jacop.search.IndomainMin;
import org.jacop.search.Search;
import org.jacop.search.SelectChoicePoint;
import org.jacop.search.SimpleSelect;
import org.jacop.search.SmallestDomain;

public class Sat {

	public static void main(String[] args) throws FileNotFoundException, IOException {
		Store store = new Store();
		SatWrapper satWrapper = new SatWrapper();
		store.impose(satWrapper);

		String linea = "";

		int columnasMatriz = 0;
		int filasMatriz = 0;
		int numeroSerpientes = Integer.parseInt(args[1]);
		BooleanVar serpientes[][][];
		BooleanVar al[][];
		BooleanVar allVariables[];
		int alLiterals[][];
		int snakeLiterals[][][];
		int casillasVacias = 0;
		int i = 0;

		FileReader f = new FileReader(args[0]);
		BufferedReader b = new BufferedReader(f);

		while((linea = b.readLine()) != null) {
			if(columnasMatriz == 0) columnasMatriz = linea.length();
			filasMatriz++;
		}

		f = new FileReader(args[0]);
		b = new BufferedReader(f);
		char fichero[][] = new char[filasMatriz][columnasMatriz];
		alLiterals = new int[filasMatriz][columnasMatriz];
		snakeLiterals = new int[numeroSerpientes][filasMatriz][columnasMatriz];

		while((linea = b.readLine()) != null) {
			for(int j = 0; j < linea.length(); j++) {
				fichero[i][j] = linea.charAt(j);
				if(linea.charAt(j) == ' ') casillasVacias++;
			}
			i++;
		}

		serpientes = new BooleanVar[numeroSerpientes][filasMatriz][columnasMatriz];
		al = new BooleanVar[filasMatriz][columnasMatriz];
		allVariables = new BooleanVar[casillasVacias * (numeroSerpientes + 1)];
		i = 0;

		for(int k = 0; k < numeroSerpientes + 1; k++) {
			if(k != 0) serpientes[k - 1] = new BooleanVar[filasMatriz][columnasMatriz];
			for(int l = 0; l < filasMatriz; l++) {
				for(int m = 0; m < columnasMatriz; m++) {
					if(fichero[l][m] == ' ') {
						if(k == 0) {
							al[l][m] = new BooleanVar(store, "\n Al" + l + m);
							satWrapper.register(al[l][m]);
							alLiterals[l][m] = satWrapper.cpVarToBoolVar(al[l][m], 1, true);
							allVariables[i] = al[l][m];
							i++;
						} else {
							serpientes[k - 1][l][m] = new BooleanVar(store, "\n Serpiente" + k + l + m);
							satWrapper.register(serpientes[k - 1][l][m]);
							snakeLiterals[k - 1][l][m] = satWrapper.cpVarToBoolVar(serpientes[k - 1][l][m], 1, true);
							allVariables[i]	= serpientes[k - 1][l][m];
							i++;
						}
					}
				}
			}
		}

		b.close();

		for(int k = 0; k < numeroSerpientes + 1; k++) {
			if(k == 0) {
				addClause(satWrapper, alLiterals, fichero);
				addSingleElementClause(satWrapper, snakeLiterals, alLiterals, fichero);
				addAlClause(satWrapper, snakeLiterals, alLiterals, fichero);
				addSnakeRowClause(satWrapper, snakeLiterals, fichero);
			} else {
				addClause(satWrapper, snakeLiterals[k - 1], fichero);
			}
		}

	    Search<BooleanVar> search = new DepthFirstSearch<BooleanVar>();
		SelectChoicePoint<BooleanVar> select = new SimpleSelect<BooleanVar>(allVariables,new SmallestDomain<BooleanVar>(), new IndomainMin<BooleanVar>());
		Boolean result = search.labeling(store, select);

		if (result) {
			PrintWriter writer = new PrintWriter("lab1_parte1.lab.output", "UTF-8");
			char c = ' ';
			for(int k = 0; k < filasMatriz; k++) {
				for(int l = 0; l < columnasMatriz; l++) {
						if(fichero[k][l] == ' ') {
							if(al[k][l].value() == 1) {
								c = 'A';
							//} else if(serpientes[0][k][l].value() == 1){
								//c = 'S';
							} else {
								for(int m = 0; m < numeroSerpientes; m++) {
									if(serpientes[m][k][l].value() == 1) {
										c = 'S';
									}
								}
							}
							writer.print(c);
							c= ' ';
						} else {
							writer.print(fichero[k][l]);
						}
				}
				writer.println();
			}
			writer.close();
		}else{
			System.out.println("Solution not found");
		}
	}

	public static void addClause(SatWrapper satWrapper, int[][] literals, char[][] fichero) {
		IntVec clause = new IntVec(satWrapper.pool);
		for(int k = 0; k < fichero.length; k++) {
			for(int l = 0; l < fichero[k].length; l++) {
				if(fichero[k][l] == ' ') {
					clause.add(literals[k][l]);
				}
			}
		}
		satWrapper.addModelClause(clause.toArray());
	}

	public static void addSnakeRowClause(SatWrapper satWrapper, int[][][] literals, char[][] fichero) {
		IntVec clause;
		for(int k = 0; k < fichero.length; k++) {
			for(int l = 0; l < fichero[k].length; l++) {
				if(fichero[k][l] == ' ') {
					for(int m = 0; m < literals.length; m++) {
						for(int i = 0; i < literals.length; i++) {
							for(int n = 0; n < fichero[k].length; n++) {
								clause = new IntVec(satWrapper.pool);
								if((n != l) && (i != m) && (m < i) && (fichero[k][n] == ' ')) {
									clause.add(-literals[i][k][n]);
									clause.add(-literals[m][k][l]);
									satWrapper.addModelClause(clause.toArray());
								}
							}
						}
					}
				}
			}
		}
	}

	public static void addSingleElementClause(SatWrapper satWrapper, int[][][] snakeLiterals, int[][] alLiterals,char[][] fichero) {
		IntVec clause;
		for(int k = 0; k < fichero.length; k++) {
			for(int l = 0; l < fichero[k].length; l++) {
				if(fichero[k][l] == ' ') {
					for(int m = 0; m < snakeLiterals.length; m++) {
						for(int i = 0; i < snakeLiterals.length; i++) {
							if((m != i) && (m < i)) {
								clause = new IntVec(satWrapper.pool);
								clause.add(-snakeLiterals[m][k][l]);
								clause.add(-snakeLiterals[i][k][l]);
								satWrapper.addModelClause(clause.toArray());
							}
						}
						clause = new IntVec(satWrapper.pool);
						clause.add(-alLiterals[k][l]);
						clause.add(-snakeLiterals[m][k][l]);
						satWrapper.addModelClause(clause.toArray());
					}
				}
			}
		}
	}

	public static void addAlClause(SatWrapper satWrapper, int[][][] snakeLiterals, int[][] alLiterals,char[][] fichero) {
		IntVec clause;
		for(int k = 0; k < fichero.length; k++) {
			for(int l = 0; l < fichero[k].length; l++) {
				if(fichero[k][l] == ' ') {
					for(int m = 0; m < snakeLiterals.length; m++) {
						for(int p = 0; p < fichero[k].length; p++) {
							if((fichero[k][p] == ' ') && (p != l)) {
								clause = new IntVec(satWrapper.pool);
								clause.add(-alLiterals[k][l]);
							    clause.add(-snakeLiterals[m][k][p]);
								satWrapper.addModelClause(clause.toArray());
							}
						}
						for(int n = 0; n < fichero.length; n++) {
							if((fichero[n][l] == ' ') && (n != k)) {
								clause = new IntVec(satWrapper.pool);
								clause.add(-alLiterals[k][l]);
								clause.add(-snakeLiterals[m][n][l]);
								satWrapper.addModelClause(clause.toArray());
							}
						}
					}
				}
			}
		}
	}
}
