import ctypes
from ctypes import c_char_p

from PyQt6.QtWidgets import (QApplication, QDialog, QGridLayout, QLineEdit, QPushButton)

class WidgetGallery(QDialog):

    def __init__(self, parent=None):
        super(WidgetGallery, self).__init__(parent)

        self.multiply_matrices_lib = ctypes.CDLL('../multiply-matrices.so')
        self.multiply_matrices = self.multiply_matrices_lib.MultiplyMatrices
        self.multiply_matrices.argtypes = [c_char_p, c_char_p, c_char_p]

        self.matrix_a_path_line = QLineEdit('/home/Reversean/code/coherent-chimera/course-work/matrix-a.txt')
        self.matrix_a_path_line.setObjectName('Matrix A Path')
        self.matrix_a_path_line.setPlaceholderText('Matrix A')

        self.matrix_b_path_line = QLineEdit('/home/Reversean/code/coherent-chimera/course-work/matrix-b.txt')
        self.matrix_b_path_line.setObjectName('Matrix B Path')
        self.matrix_b_path_line.setPlaceholderText('Matrix B')

        self.output_matrix_path_line = QLineEdit('/home/Reversean/code/coherent-chimera/course-work/test.txt')
        self.output_matrix_path_line.setObjectName('Output Matrix Path')
        self.output_matrix_path_line.setPlaceholderText('Output Matrix')

        multiply_button = QPushButton('Multiply Matrices')
        multiply_button.clicked.connect(self.multiply_button_click)

        layout = QGridLayout()
        layout.addWidget(self.matrix_a_path_line, 0, 0, 1, 2)
        layout.addWidget(self.matrix_b_path_line, 1, 0, 1, 2)
        layout.addWidget(self.output_matrix_path_line, 2, 0, 1, 2)
        layout.addWidget(multiply_button, 3, 0, 1, 2)
        layout.setRowStretch(4, 1)
        self.setLayout(layout)
        self.setWindowTitle("Matrices Multiplication")

    def multiply_button_click(self):
        matrix_a_path = self.matrix_a_path_line.text()
        matrix_b_path = self.matrix_b_path_line.text()
        output_matrix_path = self.output_matrix_path_line.text()
        print(matrix_a_path)
        print(matrix_b_path)
        print(output_matrix_path)
        self.multiply_matrices(
            matrix_a_path.encode('utf-8'),
            matrix_b_path.encode('utf-8'),
            output_matrix_path.encode('utf-8')
        )


if __name__ == '__main__':
    import sys

    app = QApplication(sys.argv)
    gallery = WidgetGallery()
    gallery.show()
    sys.exit(app.exec())
