use std::io;

fn main() {
    // 행렬의 크기 입력 받기
    println!("행렬의 행과 열의 크기를 입력하세요 (예: 2 3):");

    let mut input = String::new();
    io::stdin().read_line(&mut input).expect("입력 오류");
    
    let dims: Vec<usize> = input
        .trim()
        .split_whitespace()
        .map(|s| s.parse().expect("유효한 숫자가 아닙니다"))
        .collect();

    let rows = dims[0];
    let cols = dims[1];

    // 두 행렬을 동적으로 할당
    let mut matrix_a: Vec<Vec<i32>> = vec![vec![0; cols]; rows];
    let mut matrix_b: Vec<Vec<i32>> = vec![vec![0; cols]; rows];

    // 행렬 A 입력 받기
    println!("행렬 A의 요소를 입력하세요:");
    for i in 0..rows {
        for j in 0..cols {
            println!("A[{}][{}]:", i, j);
            let mut value = String::new();
            io::stdin().read_line(&mut value).expect("입력 오류");
            matrix_a[i][j] = value.trim().parse().expect("유효한 숫자가 아닙니다");
        }
    }

    // 행렬 B 입력 받기
    println!("행렬 B의 요소를 입력하세요:");
    for i in 0..rows {
        for j in 0..cols {
            println!("B[{}][{}]:", i, j);
            let mut value = String::new();
            io::stdin().read_line(&mut value).expect("입력 오류");
            matrix_b[i][j] = value.trim().parse().expect("유효한 숫자가 아닙니다");
        }
    }

    // 행렬 합산
    let mut result_matrix: Vec<Vec<i32>> = vec![vec![0; cols]; rows];

    for i in 0..rows {
        for j in 0..cols {
            result_matrix[i][j] = matrix_a[i][j] + matrix_b[i][j];
        }
    }

    // 결과 출력
    println!("행렬 A + B의 결과:");
    for row in result_matrix {
        for value in row {
            print!("{} ", value);
        }
        println!();
    }
}
