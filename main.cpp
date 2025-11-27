/*
 * ATIVIDADE PRÁTICA 01 - MÉTODO DE NEWTON-RAPHSON
 * Disciplina: Métodos Numéricos e Computacionais
 * Descrição: Implementação com Derivada Automática (Diferenças Finitas).
 */

#include <iostream>
#include <cmath>      
#include <iomanip>    
#include <functional> 
#include <string>
#include <cctype>     
#include <stdexcept>
#include <fstream>    

using namespace std;

// ==========================================================
// CLASSE: SIMPLE PARSER (Mantida idêntica para estabilidade)
// ==========================================================
class SimpleParser {
private:
    string expr;
    int pos;
    long double x_val;

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

    long double parseNumber() { 
        char c = peek();
        string numStr;
        while (isdigit(c) || c == '.') {
            numStr += get();
            c = peek();
        }
        if (numStr.empty()) return 0.0L; 
        return stold(numStr);
    }

    long double parseFactor() {
        char c = peek();
        if (isdigit(c) || c == '.') return parseNumber();
        if (c == '(') {
            get(); 
            long double result = parseExpression();
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

    long double parsePower() {
        long double left = parseFactor();
        while (peek() == '^') {
            get();
            long double right = parseFactor();
            left = pow(left, right);
        }
        return left;
    }

    long double parseTerm() {
        long double left = parsePower();
        while (peek() == '*' || peek() == '/') {
            char op = get();
            long double right = parsePower();
            if (op == '*') left *= right;
            else {
                if (abs(right) < 1e-15L) throw runtime_error("Divisao por zero detectada na expressao");
                left /= right;
            }
        }
        return left;
    }

    long double parseExpression() {
        long double left = parseTerm();
        while (peek() == '+' || peek() == '-') {
            char op = get();
            long double right = parseTerm();
            if (op == '+') left += right;
            else left -= right;
        }
        return left;
    }

public:
    string preprocess(string s) {
        string res = "";
        for (size_t i = 0; i < s.length(); i++) {
            res += s[i];
            if (i + 1 < s.length()) {
                if (isdigit(s[i]) && isalpha(s[i+1])) {
                    res += '*';
                }
            }
        }
        return res;
    }

    long double evaluate(string expression_str, long double x) {
        this->expr = preprocess(expression_str);
        this->pos = 0;
        this->x_val = x;
        return parseExpression();
    }
    
    string getFixedString(string s) {
        return preprocess(s);
    }
};

// ==========================================================
// FUNÇÕES PREDEFINIDAS
// ==========================================================
long double func1(long double x) { return pow(x, 3) - 2 * x - 5; }
long double deriv1(long double x) { return 3 * pow(x, 2) - 2; }

long double func2(long double x) { return cos(x) - x; }
long double deriv2(long double x) { return -sin(x) - 1.0L; }

long double func3(long double x) { return exp(-x) - x; }
long double deriv3(long double x) { return -exp(-x) - 1.0L; }

long double func4(long double x) { return pow(x, 3) - 9 * x + 3; }
long double deriv4(long double x) { return 3 * pow(x, 2) - 9; }

// ==========================================================
// ALGORITMO: MÉTODO DE NEWTON-RAPHSON
// ==========================================================
void newtonRaphson(
    std::function<long double(long double)> f,
    std::function<long double(long double)> df,
    long double x0,
    long double epsilon,
    int maxIter
) {
    long double x_atual = x0;
    long double x_prox;
    long double erro = epsilon + 1.0L; 
    int iter = 0;

    ofstream arquivoCsv;
    arquivoCsv.open("tabela.csv"); 
    arquivoCsv << "k,x_n,f(x_n),Erro Estimado" << endl;
    arquivoCsv << fixed << setprecision(15); 

    cout << "\n" << string(130, '-') << endl;
    cout << "TABELA DE ITERACOES (Metodo de Newton-Raphson)" << endl;
    cout << string(130, '-') << endl;
    cout << left << setw(10) << "k" 
         << left << setw(40) << "x_n" 
         << left << setw(40) << "f(x_n)" 
         << left << setw(40) << "Erro Estimado" << endl;
    cout << string(130, '-') << endl;
    cout << fixed << setprecision(12);

    while (iter < maxIter) {
        long double fx, dfx;
        
        try {
            fx = f(x_atual);
            dfx = df(x_atual); // Calcula derivada (Analítica ou Numérica)
        } catch (const exception& e) {
            cout << "\n[ERRO MATEMATICO] " << e.what() << endl;
            arquivoCsv << "ERRO," << e.what() << endl;
            arquivoCsv.close();
            return;
        }

        if (abs(dfx) < 1e-18L) { 
            cout << "\n[ERRO] Derivada anulou-se em x = " << x_atual << endl;
            arquivoCsv.close();
            return;
        }

        x_prox = x_atual - (fx / dfx);
        erro = abs(x_prox - x_atual);

        cout << left << setw(10) << iter + 1
             << left << setw(40) << x_atual
             << left << setw(40) << fx
             << left << setw(0) << erro << endl;

        arquivoCsv << (iter + 1) << "," 
                   << x_atual << "," 
                   << fx << "," 
                   << erro << endl;

        x_atual = x_prox;
        iter++;

        if (erro < epsilon) {
            cout << string(130, '-') << endl;
            cout << "\nRESULTADO FINAL:" << endl;
            cout << "Status: CONVERGIU COM SUCESSO" << endl;
            cout << "Raiz Aproximada: " << x_atual << endl;
            cout << "Total de Iteracoes: " << iter << endl;
            cout << "\n[INFO] Arquivo 'tabela.csv' gerado." << endl;
            arquivoCsv.close(); 
            return;
        }
    }

    cout << string(130, '-') << endl;
    cout << "\nRESULTADO FINAL:" << endl;
    cout << "Status: NAO CONVERGIU (Limite de iteracoes atingido)" << endl;
    cout << "Ultima aproximacao: " << x_atual << endl;
    
    arquivoCsv.close();
}

// ==========================================================
// FUNÇÃO PRINCIPAL
// ==========================================================
int main() {
    int escolha;
    long double x0, epsilon;
    int maxIter;
    SimpleParser parser;

    string userFuncStr, userDerivStr;
    std::function<long double(long double)> f_selecionada;
    std::function<long double(long double)> df_selecionada;
    string desc_f;

    cout << "=== IMPLEMENTACAO: METODO DE NEWTON-RAPHSON ===" << endl;
    cout << "1. f(x) = x^3 - 2x - 5" << endl;
    cout << "2. f(x) = cos(x) - x" << endl;
    cout << "3. f(x) = e^(-x) - x" << endl;
    cout << "4. f(x) = x^3 - 9x + 3" << endl;
    cout << "5. DIGITAR MINHA PROPRIA FUNCAO [NOVO: Derivada Automatica]" << endl;
    cout << "Opcao: ";
    cin >> escolha;

    if (escolha == 5) {
        cout << "\n--- CONFIGURACAO DE FUNCAO ---" << endl;
        cout << "Digite a funcao f(x): ";
        cin.ignore();
        getline(cin, userFuncStr);
        
        // Define a função f(x)
        f_selecionada = [&parser, userFuncStr](long double x) {
            return parser.evaluate(userFuncStr, x);
        };

        // Opção de Derivada Automática
        char calcDerivAuto;
        cout << "Deseja que o programa calcule a derivada automaticamente? (S/N): ";
        cin >> calcDerivAuto;

        if (toupper(calcDerivAuto) == 'S') {
            cout << "[Sistema] Modo Derivada Numerica ativado." << endl;
            
            // Lógica de Diferenciação Numérica (Central Difference)
            // f'(x) ~ (f(x+h) - f(x-h)) / 2h
            df_selecionada = [&parser, userFuncStr](long double x) {
                long double h = 1e-8L; // Passo pequeno
                long double fx_plus = parser.evaluate(userFuncStr, x + h);
                long double fx_minus = parser.evaluate(userFuncStr, x - h);
                return (fx_plus - fx_minus) / (2.0L * h);
            };
            
        } else {
            cout << "Digite a derivada f'(x): ";
            cin.ignore(); 
            getline(cin, userDerivStr);
            
            string fixedD = parser.getFixedString(userDerivStr);
            cout << "[Sistema] Derivada Manual: " << fixedD << endl;
            
            df_selecionada = [&parser, userDerivStr](long double x) {
                return parser.evaluate(userDerivStr, x);
            };
        }

        string fixedF = parser.getFixedString(userFuncStr);
        cout << "[Sistema] Funcao f(x): " << fixedF << endl;

    } else {
        switch (escolha) {
            case 1: f_selecionada = func1; df_selecionada = deriv1; break;
            case 2: f_selecionada = func2; df_selecionada = deriv2; break;
            case 3: f_selecionada = func3; df_selecionada = deriv3; break;
            case 4: f_selecionada = func4; df_selecionada = deriv4; break;
            default: cout << "Opcao invalida!" << endl; return 1;
        }
    }

    cout << "\n--- Parametros de Execucao ---" << endl;
    cout << "Digite o chute inicial (x0): ";
    cin >> x0;
    cout << "Digite o erro maximo permitido (epsilon): ";
    cin >> epsilon;
    cout << "Digite o numero maximo de iteracoes: ";
    cin >> maxIter;

    newtonRaphson(f_selecionada, df_selecionada, x0, epsilon, maxIter);

    cout << "\nPressione Enter para sair...";
    cin.ignore();
    cin.get();

    return 0;
}