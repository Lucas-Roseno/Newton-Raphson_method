/*
 * ATIVIDADE PRÁTICA 01 - MÉTODO DE NEWTON-RAPHSON
 * Disciplina: Métodos Numéricos e Computacionais
 * * Atualização: Correção de "2x" (Multiplicação Implícita)
 */

#include <iostream>
#include <cmath>      
#include <iomanip>    
#include <functional> 
#include <string>
#include <cctype>     
#include <stdexcept>  

using namespace std;

// ==========================================================
// CLASSE: INTERPRETADOR COM CORREÇÃO AUTOMÁTICA
// ==========================================================
class SimpleParser {
private:
    string expr;
    int pos;
    double x_val;

    // --- MÉTODOS AUXILIARES DE LEITURA ---
    char peek() {
        while (pos < expr.length() && isspace(expr[pos])) pos++;
        if (pos == expr.length()) return 0;
        return expr[pos];
    }

    char get() {
        char c = peek();
        if (pos < expr.length()) pos++;
        return c;
    }

    // --- PARSERS RECURSIVOS ---
    double parseNumber() {
        char c = peek();
        string numStr;
        while (isdigit(c) || c == '.') {
            numStr += get();
            c = peek();
        }
        if (numStr.empty()) return 0; 
        return stod(numStr);
    }

    double parseFactor() {
        char c = peek();
        
        if (isdigit(c) || c == '.') return parseNumber();
        
        if (c == '(') {
            get(); 
            double result = parseExpression();
            if (get() != ')') throw runtime_error("Esperado ')'");
            return result;
        }
        
        if (c == '-') {
            get();
            return -parseFactor();
        }

        if (isalpha(c)) {
            string name;
            while (isalpha(peek())) name += get();

            if (name == "x") return x_val;
            else if (name == "sin") return sin(parseFactor());
            else if (name == "cos") return cos(parseFactor());
            else if (name == "tan") return tan(parseFactor());
            else if (name == "exp") return exp(parseFactor()); 
            else if (name == "log") return log(parseFactor()); 
            else if (name == "sqrt") return sqrt(parseFactor());
            else throw runtime_error("Funcao desconhecida: " + name);
        }

        throw runtime_error("Caractere inesperado: " + string(1, c));
    }

    double parsePower() {
        double left = parseFactor();
        while (peek() == '^') {
            get();
            double right = parseFactor();
            left = pow(left, right);
        }
        return left;
    }

    double parseTerm() {
        double left = parsePower();
        while (peek() == '*' || peek() == '/') {
            char op = get();
            double right = parsePower();
            if (op == '*') left *= right;
            else {
                if (abs(right) < 1e-9) throw runtime_error("Divisao por zero");
                left /= right;
            }
        }
        return left;
    }

    double parseExpression() {
        double left = parseTerm();
        while (peek() == '+' || peek() == '-') {
            char op = get();
            double right = parseTerm();
            if (op == '+') left += right;
            else left -= right;
        }
        return left;
    }

public:
    // --- NOVO: PRÉ-PROCESSADOR DE STRING ---
    // Transforma "2x" em "2*x" para evitar erros do usuário
    string preprocess(string s) {
        string res = "";
        for (size_t i = 0; i < s.length(); i++) {
            res += s[i];
            // Se o atual é digito e o proximo é letra (ex: 2x, 3s de sin), insere *
            if (i + 1 < s.length()) {
                if (isdigit(s[i]) && isalpha(s[i+1])) {
                    res += '*';
                }
                // Se fecha parenteses e começa letra/numero (ex: )x ou )2 )
                // isso é mais complexo, vamos focar no 2x que foi o erro principal
            }
        }
        return res;
    }

    double evaluate(string expression_str, double x) {
        // Aplica a correção automática antes de calcular
        this->expr = preprocess(expression_str);
        this->pos = 0;
        this->x_val = x;
        return parseExpression();
    }
    
    // Método auxiliar só para mostrar ao usuário como ficou a função
    string getFixedString(string s) {
        return preprocess(s);
    }
};

// ==========================================================
// FUNÇÕES PREDEFINIDAS
// ==========================================================
double func1(double x) { return pow(x, 3) - 2 * x - 5; }
double deriv1(double x) { return 3 * pow(x, 2) - 2; }

double func2(double x) { return cos(x) - x; }
double deriv2(double x) { return -sin(x) - 1; }

double func3(double x) { return exp(-x) - x; }
double deriv3(double x) { return -exp(-x) - 1; }

double func4(double x) { return pow(x, 3) - 9 * x + 3; }
double deriv4(double x) { return 3 * pow(x, 2) - 9; }

// ==========================================================
// ALGORITMO NEWTON-RAPHSON
// ==========================================================
void newtonRaphson(
    std::function<double(double)> f,
    std::function<double(double)> df,
    double x0,
    double epsilon,
    int maxIter
) {
    double x_atual = x0;
    double x_prox;
    double erro = epsilon + 1.0;
    int iter = 0;

    cout << "\n" << string(70, '-') << endl;
    cout << "ITERACOES (Newton-Raphson)" << endl;
    cout << string(70, '-') << endl;
    cout << left << setw(5) << "k" 
         << left << setw(20) << "x_n" 
         << left << setw(20) << "f(x_n)" 
         << left << setw(20) << "Erro Estimado" << endl;
    cout << string(70, '-') << endl;
    cout << fixed << setprecision(8);

    while (iter < maxIter) {
        double fx, dfx;
        
        try {
            fx = f(x_atual);
            dfx = df(x_atual);
        } catch (const exception& e) {
            cout << "\n[ERRO DE CALCULO] " << e.what() << endl;
            return;
        }

        if (abs(dfx) < 1e-15) {
            cout << "\n[ERRO CRITICO] Derivada zerou (divisao por zero) em x=" << x_atual << endl;
            return;
        }

        x_prox = x_atual - (fx / dfx);
        erro = abs(x_prox - x_atual);

        cout << left << setw(5) << iter + 1
             << left << setw(20) << x_atual
             << left << setw(20) << fx
             << left << setw(20) << erro << endl;

        x_atual = x_prox;
        iter++;

        if (erro < epsilon) {
            cout << string(70, '-') << endl;
            cout << "\nRESULTADO FINAL:" << endl;
            cout << "Status: CONVERGIU" << endl;
            cout << "Raiz Aproximada: " << x_atual << endl;
            cout << "Total de Iteracoes: " << iter << endl;
            return;
        }
    }

    cout << string(70, '-') << endl;
    cout << "\nRESULTADO FINAL:" << endl;
    cout << "Status: NAO CONVERGIU (Limite de iteracoes)" << endl;
    cout << "Ultima aproximacao: " << x_atual << endl;
}

// ==========================================================
// MAIN
// ==========================================================
int main() {
    int escolha;
    double x0, epsilon;
    int maxIter;
    SimpleParser parser;

    string userFuncStr, userDerivStr;
    std::function<double(double)> f_selecionada;
    std::function<double(double)> df_selecionada;
    string desc_f;

    cout << "=== METODO DE NEWTON-RAPHSON ===" << endl;
    cout << "1. f(x) = x^3 - 2x - 5" << endl;
    cout << "2. f(x) = cos(x) - x" << endl;
    cout << "3. f(x) = e^(-x) - x" << endl;
    cout << "4. f(x) = x^3 - 9x + 3" << endl;
    cout << "5. DIGITAR MINHA PROPRIA FUNCAO" << endl;
    cout << "Opcao: ";
    cin >> escolha;

    if (escolha == 5) {
        cout << "\n--- GUIA RAPIDO ---" << endl;
        cout << "Use: + - * / ^" << endl;
        cout << "Ex:  x^2 - 4" << endl;
        cout << "Obs: O programa corrige '2x' para '2*x' automaticamente." << endl;
        cout << "------------------------------------" << endl;

        cout << "Digite a funcao f(x): ";
        cin.ignore(); 
        getline(cin, userFuncStr);

        cout << "Digite a derivada f'(x): ";
        getline(cin, userDerivStr);

        // Mostra a correção para o usuário confirmar
        string fixedF = parser.getFixedString(userFuncStr);
        string fixedD = parser.getFixedString(userDerivStr);
        cout << "\n[Interpretador] Entendido como:" << endl;
        cout << "Funcao:   " << fixedF << endl;
        cout << "Derivada: " << fixedD << endl;

        desc_f = fixedF;

        f_selecionada = [&parser, userFuncStr](double x) {
            return parser.evaluate(userFuncStr, x);
        };
        df_selecionada = [&parser, userDerivStr](double x) {
            return parser.evaluate(userDerivStr, x);
        };

    } else {
        switch (escolha) {
            case 1: f_selecionada = func1; df_selecionada = deriv1; desc_f = "x^3 - 2x - 5"; break;
            case 2: f_selecionada = func2; df_selecionada = deriv2; desc_f = "cos(x) - x"; break;
            case 3: f_selecionada = func3; df_selecionada = deriv3; desc_f = "e^(-x) - x"; break;
            case 4: f_selecionada = func4; df_selecionada = deriv4; desc_f = "x^3 - 9x + 3"; break;
            default: cout << "Opcao invalida!" << endl; return 1;
        }
    }

    cout << "\n--- Parametros ---" << endl;
    cout << "Chute inicial (x0): ";
    cin >> x0;
    cout << "Erro maximo (epsilon, Ex = 0.0001): ";
    cin >> epsilon;
    cout << "Maximo de iteracoes: ";
    cin >> maxIter;

    newtonRaphson(f_selecionada, df_selecionada, x0, epsilon, maxIter);

    cout << "\nPressione Enter para sair...";
    cin.ignore();
    cin.get();

    return 0;
}