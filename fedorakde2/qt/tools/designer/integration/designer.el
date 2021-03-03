(defvar designer-exec "/home/reggie/troll/qt-main/bin/designer")

(defun designer-open-ui-file ()
  (interactive)
  (defvar procname "designer")
  (start-process procname nil designer-exec "-style" "windows" "-client" buffer-file-name)
)

(defun designer-create-ui-file (&optional arg)
  (interactive "FNew UI-File: ")
  (find-file arg)
  (insert " ")
  (save-buffer)
  (designer-open-ui-file)
)

(global-set-key [f7] 'designer-open-ui-file)
(global-set-key [f8] 'designer-create-ui-file)

