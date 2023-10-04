import java.util.Scanner;
import java.io.File;
import java.util.Hashtable;
import java.util.ArrayList;
import java.util.Random;
import java.util.Arrays;
import java.util.List;

public class CFG {
    private static String[] variables;
    private static String[] terminals;
    private static Hashtable<String, ArrayList<String>> productions;
    private static String startSymbol;
    private static int numExpansions;
    private static final int MAX_ITERS = 25;    // maximum expansions allowed (adjust as needed)
    private static final int SEED = 10;         // for testing
    private static final String ERROR_MSG = "Usage: CFG cfgFile.cfg numRandomStrings";

    public static void main(String[] args) throws Exception {
        try {
            if (args.length != 2) {
                throw new Exception(ERROR_MSG);
            }
            String filename = args[0];
            if (!filename.endsWith(".cfg")) {
                throw new Exception(ERROR_MSG);
            }
            int numRandomStrings = Integer.parseInt(args[1]);
            if (numRandomStrings <= 0) {
                throw new Exception("Number of random strings must be greater than 0.");
            }

            Scanner stdin = new Scanner(new File(filename));
            variables = stdin.nextLine().split(",");
            terminals = stdin.nextLine().split(",");
            productions = new Hashtable<>();
            String prevVariable = null;
            String line = stdin.nextLine();
            while (line.contains(" -> ")) {
                String[] sides = line.split(" -> ");
                String variable = sides[0];
                if (variable.equals(prevVariable)) {
                    productions.get(variable).add(sides[1]);
                }
                else {
                    ArrayList<String> stringsOfSymbols = new ArrayList<>();
                    stringsOfSymbols.add(sides[1]);
                    productions.put(variable, stringsOfSymbols);
                }
                prevVariable = variable;
                line = stdin.nextLine();
            }
            startSymbol = line;
            stdin.close();

            Random rng = new Random(SEED);
            ArrayList<String> randomStrings = new ArrayList<>();
            while (randomStrings.size() != numRandomStrings) {
                String randString = generate(rng);
                if (randString == null) {
                    continue;
                }
                if (!randomStrings.contains(randString)) {
                    randomStrings.add(randString);
                    System.out.print(String.format("(%d) ", randomStrings.size()));
                    printString(randString);
                }
            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
            System.exit(1);
        }
    }

    private static String generate(Random rng) {
        String generatedString = startSymbol;
        numExpansions = 0;
        boolean allTerminals = false;
        while (!allTerminals) {
            String result = applyRule(generatedString, rng);
            if (numExpansions == MAX_ITERS) {
                return null;
            }
            if (result != null) {
                generatedString = result;
            }
            else {
                allTerminals = true;
            }
        }
        return generatedString;
    }

    private static String applyRule(String s, Random rng) {
        ArrayList<String> symbols = new ArrayList<>(Arrays.asList(s.split(" ")));
        List<String> varList = Arrays.asList(variables);
        for (int i = 0; i < symbols.size(); i++) {
            String symbol = symbols.get(i);
            if (varList.contains(symbol)) {
                ArrayList<String> sa = productions.get(symbol);
                String replacementString = sa.get(rng.nextInt(sa.size()));
                for (String var : varList) {
                    if (replacementString.contains(var)) {
                        numExpansions++;
                        break;
                    }
                }
                if (replacementString.equals("_EMPTY_")) {
                    symbols.remove(i);
                }
                else {
                    symbols.set(i, replacementString);
                }
                return String.join(" ", symbols);
            }
        }
        return null;
    }

    private static void printString(String s) {
        if (s.isEmpty()) {
            System.out.println("_EMPTY_");
        }
        else {
            System.out.println(s);
        }
    }
}
