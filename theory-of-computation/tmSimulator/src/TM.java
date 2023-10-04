import java.util.Scanner;
import java.io.File;
import java.util.Hashtable;
import java.util.ArrayList;
import java.util.Arrays;

public class TM {
    private static String[] states;
    private static String[] inputAlphabet;
    private static String[] tapeAlphabet;
    private static Hashtable<String, String> transitionStates;
    private static String startState;
    private static String acceptState;
    private static String rejectState;
    private static ArrayList<String> tape;
    private static int tapeHeadPos;
    private static final String BLANK = "_";
    private static final String ERROR_MSG = "Usage: TM tmFile.tm inputString [\"verbose\"]";

    public static void main(String[] args) {
        try {
            if (!(args.length == 2 || args.length == 3)) {
                throw new Exception(ERROR_MSG);
            }
            String filename = args[0];
            if (!filename.endsWith(".tm")) {
                throw new Exception(ERROR_MSG);
            }
            String inputString = args[1];
            boolean verbose = args.length == 3 && args[2].equals("verbose");

            Scanner stdin = new Scanner(new File(filename));
            states = stdin.nextLine().split(",");
            inputAlphabet = stdin.nextLine().split(",");
            tapeAlphabet = stdin.nextLine().split(",");
            transitionStates = new Hashtable<>();
            String line = stdin.nextLine();
            while (line.contains("->")) {
                String[] functionValues = line.split("->");
                String input = functionValues[0];
                String output = functionValues[1];
                transitionStates.put(input, output);
                line = stdin.nextLine();
            }
            startState = line;
            acceptState = stdin.nextLine();
            rejectState = stdin.nextLine();
            stdin.close();

            if (isMalformed(inputString)) {
                System.out.println("Malformed.");
                return;
            }

            initTape(inputString);
            String currentState = startState;
            printTape(currentState);
            boolean halt = false;
            while (!halt) {
                String inputSymbol = readFromTape();
                currentState = transition(String.format("(%s,%s)", currentState, inputSymbol));
                if (verbose) {
                    printTape(currentState);
                }
                if (currentState.equals(acceptState)) {
                    printTape(currentState);
                    System.out.println("Accepted!");
                    halt = true;
                }
                else if (currentState.equals(rejectState)) {
                    System.out.println("Rejected.");
                    halt = true;
                }
            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
            System.exit(1);
        }
    }

    private static String transition(String input) {
        String output = transitionStates.get(input);
        String[] outputValues = removeParens(output).split(",");
        String outputSymbol = outputValues[1];
        writeToTape(outputSymbol);
        String direction = outputValues[2];
        switch (direction) {
            case "R":
                moveRight();
                break;
            case "L":
                moveLeft();
                break;
        }
        String nextState = outputValues[0];
        return nextState;
    }

    private static void initTape(String inputString) {
        tape = new ArrayList<>();
        for (int i = 0; i < inputString.length(); i++) {
            tape.add(inputString.substring(i, i+1));
        }
        tape.add(BLANK);
        tapeHeadPos = 0;
    }

    private static String readFromTape() {
        return tape.get(tapeHeadPos);
    }

    private static void writeToTape(String s) {
        tape.set(tapeHeadPos, s);
    }

    private static void moveRight() {
        if (tapeHeadPos == tape.size()-1) {
            tape.add(BLANK);
        }
        tapeHeadPos++;
    }

    private static void moveLeft() {
        if (tapeHeadPos == 0) {
            tape.add(0, BLANK);
        }
        else {
            tapeHeadPos--;
        }
    }

    private static void printTape(String currentState) {
        int indexOfFirstNonBlankSymbol = getIndexOfFirstNonBlankSymbol();
        int indexOfLastNonBlankSymbol = getIndexOfLastNonBlankSymbol();
        for (int i = 0; i < tape.size(); i++) {
            if (i == tapeHeadPos) {
                System.out.print(currentState + " ");
            }
            if (i >= indexOfFirstNonBlankSymbol && i <= indexOfLastNonBlankSymbol) {
                System.out.print(tape.get(i) + " ");
            }
        }
        if (tapeHeadPos == indexOfLastNonBlankSymbol) {
            System.out.print(BLANK);
        }
        System.out.println();
    }
    
    private static int getIndexOfFirstNonBlankSymbol() {
        for (int i = 0; i < tape.size(); i++) {
            if (!tape.get(i).equals(BLANK)) {
                return i;
            }
        }
        return -1;
    }

    private static int getIndexOfLastNonBlankSymbol() {
        for (int i = tape.size()-1; i >= 0; i--) {
            if (!tape.get(i).equals(BLANK)) {
                return i;
            }
        }
        return -1;
    }

    private static boolean isMalformed(String inputString) {
        for (int i = 0; i < inputString.length(); i++) {
            String inputSymbol = inputString.substring(i, i+1);
            if (!Arrays.asList(inputAlphabet).contains(inputSymbol)) {
                return true;
            }
        }
        return false;
    }

    private static String removeParens(String s) {
        return s.replace("(", "").replace(")", "");
    }
}
