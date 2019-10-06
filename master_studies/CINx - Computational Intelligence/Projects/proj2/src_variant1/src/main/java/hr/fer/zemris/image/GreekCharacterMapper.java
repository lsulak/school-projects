package hr.fer.zemris.image;

/**
 * @author Krisztian Benko
 */
public class GreekCharacterMapper implements ICharacterMapper {

    @Override
    public int mapCharacter(char character) {
        switch (character) {
            case 'Α':
            case 'α':
                return 0;
            case 'Β':
            case 'β':
                return 1;
            case 'Γ':
            case 'γ':
                return 2;
            case 'Δ':
            case 'δ':
                return 3;
            case 'Ε':
            case 'ε':
                return 4;
            case 'Ζ':
            case 'ζ':
                return 5;
            case 'Η':
            case 'η':
                return 6;
            case 'Θ':
            case 'θ':
                return 7;
            case 'Ι':
            case 'ι':
                return 8;
            case 'Κ':
            case 'κ':
                return 9;
            case 'Λ':
            case 'λ':
                return 10;
            case 'Μ':
            case 'μ':
                return 11;
            case 'Ν':
            case 'ν':
                return 12;
            case 'Ξ':
            case 'ξ':
                return 13;
            case 'Ο':
            case 'ο':
                return 14;
            case 'Π':
            case 'π':
                return 15;
            case 'Ρ':
            case 'ρ':
                return 16;
            case 'Σ':
            case 'σ':
            case 'ς':
                return 17;
            case 'Τ':
            case 'τ':
                return 18;
            case 'Υ':
            case 'υ':
                return 19;
            case 'φ':
            case 'Φ':
                return 20;
            case 'Χ':
            case 'χ':
                return 21;
            case 'Ψ':
            case 'ψ':
                return 22;
            case 'Ω':
            case 'ω':
                return 23;
            default:
                return -1;
        }
    }

    @Override
    public char mapInteger(int integer) {
        switch (integer) {
            case 0:
                return 'Α';
            case 1:
                return 'Β';
            case 2:
                return 'Γ';
            case 3:
                return 'Δ';
            case 4:
                return 'Ε';
            case 5:
                return 'Ζ';
            case 6:
                return 'Η';
            case 7:
                return 'Θ';
            case 8:
                return 'I';
            case 9:
                return 'Κ';
            case 10:
                return 'Λ';
            case 11:
                return 'Μ';
            case 12:
                return 'Ν';
            case 13:
                return 'Ξ';
            case 14:
                return 'Ο';
            case 15:
                return 'Π';
            case 16:
                return 'Ρ';
            case 17:
                return 'Σ';
            case 18:
                return 'Τ';
            case 19:
                return 'Υ';
            case 20:
                return 'φ';
            case 21:
                return 'Χ';
            case 22:
                return 'Ψ';
            case 23:
                return 'Ω';
            default:
                return ' ';
        }
    }

    @Override
    public int getSize() {
        return 24;
    }
}
