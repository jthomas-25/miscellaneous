import java.util.Scanner;
import java.io.File;
import java.util.Hashtable;
import java.util.Arrays;

public class DFA {
    private static final String ERROR_MSG = "Usage: DFA dfaFile.dfa inputString";
    private static final boolean VERBOSE = false;

    public static void main(String[] args) {
        try {
            if (args.length != 2) {
                throw new Exception(ERROR_MSG);
            }
            String filename = args[0];
            if (!filename.endsWith(".dfa")) {
                throw new Exception(ERROR_MSG);
            }
            String inputString = args[1];

            Scanner stdin = new Scanner(new File(filename));
            String[] states = stdin.nextLine().split(",");
            String[] alphabet = stdin.nextLine().split(",");
            String startState = stdin.nextLine();
            String[] acceptStates = stdin.nextLine().split(",");
            Hashtable<String, String> transitionStates = new Hashtable<>();
            while (stdin.hasNextLine()) {
                String[] functionValues = stdin.nextLine().split("->");
                String input = functionValues[0];
                String output = functionValues[1];
                transitionStates.put(input, output);
            }
            stdin.close();

            String currentState = startState;
            for (int i = 0; i < inputString.length(); i++) {
                String inputSymbol = inputString.substring(i, i+1);
                if (!Arrays.asList(alphabet).contains(inputSymbol)) {
                    System.out.println("Malformed.");
                    return;
                }
                String nextState = transitionStates.get(String.format("(%s,%s)", currentState, inputSymbol));
                if (VERBOSE) {
                    System.out.println(String.format("Current state: %s, Current symbol: %s, Next state: %s", currentState, inputSymbol, nextState));
                }
                currentState = nextState;
            }
            if (Arrays.asList(acceptStates).contains(currentState)) {
                System.out.println("Accepted!");
            }
            else {
                System.out.println("Rejected.");
            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
            System.exit(1);
        }
    }
}
