pub fn is_number(number: &str) -> bool
{
    let trimmed = number.trim();
    if trimmed.is_empty() {
        return false;
    }
    let mut chars = trimmed.chars().peekable();
    let mut has_dot = false;
    let mut has_digit = false;
    let mut digit_before_dot = false;
    if chars.peek() == Some(&'-') {
        chars.next();
    }
    while let Some(ch) = chars.next() {
        if ch.is_digit(10) {
            has_digit = true;
            if !has_dot {
                digit_before_dot = true;
            }
        } else if ch == '.' {
            if has_dot {
                return false;
            }
            if !digit_before_dot {
                return false;
            }
            has_dot = true;
        } else {
            return false;
        }
    }
    has_digit && (!has_dot || digit_before_dot)
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_valid_integer() {
        assert_eq!(is_number("42"), true);
        assert_eq!(is_number("0"), true);
        assert_eq!(is_number("12345"), true);
    }
    
    #[test]
    fn test_valid_negative_integer() {
        assert_eq!(is_number("-42"), true);
        assert_eq!(is_number("-0"), true);
        assert_eq!(is_number("-12345"), true);
    }
    
    #[test]
    fn test_valid_float() {
        assert_eq!(is_number("42.42"), true);
        assert_eq!(is_number("0.5"), true);
        assert_eq!(is_number("123.456"), true);
    }
    
    #[test]
    fn test_valid_negative_float() {
        assert_eq!(is_number("-0.4"), true);
        assert_eq!(is_number("-42.42"), true);
        assert_eq!(is_number("-123.456"), true);
    }
    
    #[test]
    fn test_whitespace_handling() {
        assert_eq!(is_number(" 123.456 "), true);
        assert_eq!(is_number("  42  "), true);
        assert_eq!(is_number("\t123.456"), true);
    }
    
    #[test]
    fn test_invalid_multiple_dots() {
        assert_eq!(is_number("42.24.42"), false);
        assert_eq!(is_number("1.2.3"), false);
    }
    
    #[test]
    fn test_invalid_text() {
        assert_eq!(is_number("yeet"), false);
        assert_eq!(is_number("abc"), false);
        assert_eq!(is_number(""), false);
    }
    
    #[test]
    fn test_invalid_space_in_number() {
        assert_eq!(is_number("123 .456"), false);
        assert_eq!(is_number("123. 456"), false);
        assert_eq!(is_number("12 3.456"), false);
    }
    
    #[test]
    fn test_invalid_dot_without_digit_before() {
        assert_eq!(is_number("-.8"), false);
        assert_eq!(is_number(".5"), false);
    }
    
    #[test]
    fn test_invalid_multiple_negative_signs() {
        assert_eq!(is_number("-8-.6"), false);
        assert_eq!(is_number("--42"), false);
        assert_eq!(is_number("8.-6"), false);
    }
}
